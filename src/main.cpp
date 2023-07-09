#include <manager.h>
#include <iostream>
#include <graphics/glm_helper.h>
#include <glm/gtc/matrix_inverse.hpp>
#include <GameHelper/camera.h>

#include "board.h"

class App {
public:
    App(Manager* manager);
    void update();
    void draw();
    glm::vec3 getRayPos();
private:
    void loadAssets();
    Manager* manager;
    Resource::Texture card;
    Board board;
    std::atomic<bool> drawSumbit;
    camera::FirstPerson cam;
    glm::mat4 projInverse;
    glm::mat4 viewInverse;
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
	app.draw();
    }
}

App::App(Manager *manager) {
    this->manager = manager;
    loadAssets();
    cam = camera::FirstPerson(glm::vec3(1.0f));
    cam.setCam(glm::vec3(0.0f, -18.0f, 30.5f), 45.0f, -67, 90);
    manager->render->set3DViewMatrixAndFov(cam.getViewMatrix(), cam.getZoom(),
					   glm::vec4(cam.getPos(), 0.0f));
    projInverse = glm::inverse(manager->render->get3DProj());
    viewInverse = glm::inverse(cam.getViewMatrix());
}

void App::loadAssets() {
    card = manager->render->LoadTexture("textures/card.png");
    board = Board(manager->render);
    manager->render->LoadResourcesToGPU();
    manager->render->UseLoadedResources();
}

glm::vec3 App::getRayPos() {
    glm::vec2 pos = manager->mousePos();
    float xPos = ((pos.x * 2.0f) / manager->winWidth) - 1.0f;
    float yPos = ((pos.y * 2.0f) / manager->winHeight) - 1.0f;
    if(manager->render->getRenderFramework() == RenderFramework::OPENGL)
	yPos *= -1;

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

void App::update() {
    if(manager->input.kb.press(GLFW_KEY_ESCAPE))
	glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
    //cam.update(manager->input, manager->timer);
    board.intersect(getRayPos());
}

void App::draw() {
    manager->render->Begin3DDraw();

    board.draw(manager->render);
    
    manager->render->Begin2DDraw();

    manager->render->DrawQuad(card, glmhelper::calcMatFromRect(
    				      glm::vec4(100, 400, 100, 150), 0.0f));
    
    manager->render->EndDraw(this->drawSumbit);
}
