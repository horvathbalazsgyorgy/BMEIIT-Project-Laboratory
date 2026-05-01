#include "framework/framework.h"
#include "code/scene/builder/builder3d.h"

using namespace Framework;

int main() {
    GLApplication::initializeGL(4, 3);
    GLApplication::createWindow(1280, 720);

    auto* builder = new Builder3D();
    auto* scene = new Scene(builder);
    scene->buildScene();

    GLApplication::render(scene);
    return 0;
}