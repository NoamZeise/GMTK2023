#include <manager.h>
#include <iostream>
#include <graphics/glm_helper.h>

class App {
public:
    App(Manager* manager);
    void update();
    void draw();
private:
    void loadAssets();
    Manager* manager;
    Resource::Texture tex;
    std::atomic<bool> drawSumbit;
};

void update(Manager &manager);

int main() {
    ManagerState managerState;
    managerState.windowName = "GMTK2023";
    Manager manager(managerState);
    App app(&manager);
    manager.render->LoadResourcesToGPU();
    manager.render->UseLoadedResources();
    while(!glfwWindowShouldClose(manager.window)) {
	manager.update();
	app.update();
	app.draw();
    }
}

App::App(Manager *manager) {
    this->manager = manager;
    loadAssets();
}

void App::loadAssets() {
    tex = manager->render->LoadTexture("textures/buddah.jpg");
}

void App::update() {
    if(manager->input.kb.press(GLFW_KEY_ESCAPE))
	glfwSetWindowShouldClose(manager->window, GLFW_TRUE);
}

void App::draw() {
    manager->render->Begin2DDraw();

    manager->render->DrawQuad(tex, glmhelper::calcMatFromRect(
				      glm::vec4(100, 100, 100, 100), 0.0f));
    
    manager->render->EndDraw(this->drawSumbit);
}
