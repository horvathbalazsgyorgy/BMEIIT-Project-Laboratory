#include "framework/framework.h"
#include "test/builder2d.h"

using namespace Framework;

int main() {
    GLApplication::initializeGL(3, 3);
    GLApplication::createWindow(600, 800);

    SceneBuilder* builder = new Builder2D();
    Scene* scene = new Scene(builder);
    scene->buildScene();

    GLApplication::render(scene);
    return 0;
}