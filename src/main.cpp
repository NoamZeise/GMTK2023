#include <manager.h>
#include <iostream>
#include <graphics/glm_helper.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <GameHelper/camera.h>

#include "board.h"
#include "card.h"

class App {
public:
    App(Manager* manager);
    ~App();
    void update();
    void draw();
    glm::vec3 getRayPos();
private:
    void loadAssets();
    void calcInvScreen();
    void nextLevel();
    Manager* manager;
    Resource::Texture updateTex;
    glm::mat4 updateRect;

    Resource::Texture completeScreen;
    Resource::Texture loseScreen;
    Resource::Texture winScreen;
    glm::mat4 messageMat;


    float MessageTimer = 3000.0f;
    float MessageDelay = 3000.0f;
    bool victory = false;

    bool gameComplete = false;
    
    Level currentLevel;
    int currentLevelNumber = -1;
    std::vector<Level> levels;
    Board board;
    Hand hand;
    std::atomic<bool> drawSumbit;
    camera::FirstPerson cam;
    glm::vec2 prevFinalRes;
    float ratioDiff;
    glm::mat4 screenInverse;
    glm::mat4 projInverse;
    glm::mat4 viewInverse;

    float boardUpdateDelay = 1000.0f;
    float boardUpdateTimer = 0.0f;
};

void update(Manager &manager);

int main() {
    ManagerState managerState;
    managerState.windowName = "GMTK2023";
    managerState.cursor = cursorState::normal;
    managerState.framebufferWidth = 800;
    managerState.framebufferHeight = 600;
    Manager *manager;
    try {
	manager = new Manager(managerState);
    } catch(std::exception &e) {
	std::cerr << "Exception: " << e.what() << std::endl;
	manager = new Manager(RenderFramework::OPENGL, managerState);
    }
    App app(manager);
    while(!glfwWindowShouldClose(manager->window)) {
	manager->update();
	app.update();
	if(manager->winWidth != 0 && manager->winHeight !=0)
	    app.draw();
    }
}

void App::calcInvScreen() {
    prevFinalRes = glm::vec2(manager->winWidth, manager->winHeight);
    glm::vec2 offscreen = manager->render->getTargetResolution();
    glm::mat4 off = glmhelper::calcFinalOffset(offscreen, prevFinalRes);
    screenInverse = glm::inverse(off);

    float offRatio = offscreen.x / offscreen.y;
    float finalRatio = prevFinalRes.y / prevFinalRes.x;
    ratioDiff = offRatio * finalRatio;
}

App::App(Manager *manager) {
    this->manager = manager;
    loadAssets();
    cam = camera::FirstPerson(glm::vec3(1.0f));
    cam.setCam(glm::vec3(0.0f, -18.0f, 30.5f), 45.0f, -67, 90);
    manager->render->set3DViewMatrixAndFov(cam.getViewMatrix(), cam.getZoom(),
					   glm::vec4(cam.getPos(), 0.0f));
    calcInvScreen();
    projInverse = glm::inverse(manager->render->get3DProj());
    viewInverse = glm::inverse(cam.getViewMatrix());
    levels = loadLevels();
    std::cout << "loaded " << levels.size() << " levels\n";
    nextLevel();
}

App::~App() {
    delete manager;
}

void App::loadAssets() {
    updateTex = manager->render->LoadTexture("textures/update.png");
    updateRect = glmhelper::calcMatFromRect(glm::vec4(0, 0, updateTex.dim.x, updateTex.dim.y),
					    0.0f, 1.0f);
    completeScreen = manager->render->LoadTexture("textures/complete.png");
    loseScreen = manager->render->LoadTexture("textures/retry.png");
    winScreen = manager->render->LoadTexture("textures/winner.png");
    messageMat = glmhelper::calcMatFromRect(glm::vec4(200.0f, 100.0f, completeScreen.dim.x, completeScreen.dim.y),
					    0.0f, 1.0f);
    board = Board(manager->render);
    hand = Hand(manager);
    manager->render->LoadResourcesToGPU();
    manager->render->UseLoadedResources();
}

glm::vec3 App::getRayPos() {
    glm::vec2 pos = manager->mousePos();
    if(prevFinalRes != glm::vec2(manager->winWidth, manager->winHeight))
	calcInvScreen();
    glm::vec4 p = screenInverse * glm::vec4(pos.x, pos.y, 0.0f, 1.0f);
    
    pos = glm::vec2(p.x, p.y);
    float xPos = ((pos.x * 2.0f) / manager->render->getTargetResolution().x) - 1.0f;
    float yPos = ((pos.y * 2.0f) / manager->render->getTargetResolution().y) - 1.0f;

    if(ratioDiff > 1)
	yPos -= ratioDiff - 1;
    if(ratioDiff < 1)
	xPos -= (1 / ratioDiff) - 1;

    if(manager->render->getRenderFramework() == RenderFramework::OPENGL)
	yPos *= -1;

    manager->correctedMouse.x = ((xPos + 1.0f) / 2.0f) * manager->render->getTargetResolution().x;
    manager->correctedMouse.y = (((manager->render->getRenderFramework()
				   == RenderFramework::OPENGL ? -1*yPos : yPos) + 1.0f)
				 / 2.0f) * manager->render->getTargetResolution().y;
    
    glm::vec4 rayClip(xPos, yPos, -1.0f, 1.0f); // NDS
    //to cam space
    glm::vec4 rayCam = projInverse * rayClip;
    rayCam.z = -1.0f;
    rayCam.w = 0.0f;
    //to world space
    glm::vec4 rayWorld4 = viewInverse * rayCam;
    glm::vec3 rayWorld(rayWorld4.x, rayWorld4.y, rayWorld4.z);
    rayWorld = glm::normalize(rayWorld);
    //get plane intersection coord
    glm::vec3 planeNormal(0.0f, 0.0f, 1.0f);
    float denom = glm::dot(rayWorld, planeNormal);
    if(denom != 0) {
	float t = -(glm::dot(cam.getPos(), planeNormal) + 0.0f) / denom;
	glm::vec3 hit = cam.getPos() + rayWorld * t;
	return hit;
    }
    return glm::vec3(10000.0f);
}

void App::nextLevel() {
    currentLevelNumber+=1;
    if(currentLevelNumber >= levels.size())
	gameComplete = true;
    else
	currentLevel = levels[currentLevelNumber];
    hand.setLevel(currentLevel, board);
}

void App::update() {
    if(manager->input.kb.press(GLFW_KEY_ESCAPE))
	glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
 
    if(gameComplete) {
	if(manager->input.m.press(GLFW_MOUSE_BUTTON_LEFT))
	    glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
    } else if(MessageTimer < MessageDelay) {
	MessageTimer += manager->timer.FrameElapsed();
	if(manager->input.m.press(GLFW_MOUSE_BUTTON_LEFT))
	    MessageTimer = MessageDelay;
    } else {
	if(!board.updating()) {
	    if(board.wonBoard() || manager->input.kb.press(GLFW_KEY_N)) {
		victory = true;
		MessageTimer = 0.0f;
		boardUpdateTimer = 0.0f;
		nextLevel();
	    } else  {
		if(hand.isEmpty()) {
		    victory = false;
		    MessageTimer = 0.0f;
		    hand.setLevel(currentLevel, board);
		}
	    }
	}
	boardUpdateTimer += manager->timer.FrameElapsed();
	if(boardUpdateTimer > boardUpdateDelay) {
	    boardUpdateTimer = 0.0f;
	    board.stepBoard();
	}
    }
    
    board.intersect(getRayPos());
    hand.update(manager, board);
}

void App::draw() {
    manager->render->Begin3DDraw();
    
    board.draw(manager->render);
    
    manager->render->Begin2DDraw();

    hand.draw(manager->render);

    if(board.updating())
	manager->render->DrawQuad(updateTex, updateRect);

    if(gameComplete)
	manager->render->DrawQuad(winScreen, messageMat);
    else if(MessageTimer < MessageDelay)
      manager->render->DrawQuad(
	      victory ? completeScreen : loseScreen, messageMat);
    
    manager->render->EndDraw(this->drawSumbit);
}
