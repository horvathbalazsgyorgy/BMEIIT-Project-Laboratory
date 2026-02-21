#include "framework/framework.h"

using namespace Framework;

int main() {
    GLApplication::initializeGL(3, 3);
    GLApplication::createWindow(600, 800);
    GLApplication::render();
    return 0;
}