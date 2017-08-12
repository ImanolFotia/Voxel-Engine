#pragma once

#include <AntTweakBar.h>

namespace gui {

bool running = true;
bool autorotate;


inline void TwEventMouseButtonGLFW3(GLFWwindow* window, int button, int action, int mods) {
    TwEventMouseButtonGLFW(button, action);
}
inline void TwEventMousePosGLFW3(GLFWwindow* window, double xpos, double ypos) {
    TwMouseMotion(int(xpos), int(ypos));
}
inline void TwEventMouseWheelGLFW3(GLFWwindow* window, double xoffset, double yoffset) {
    TwEventMouseWheelGLFW(yoffset);
}
inline void TwEventKeyGLFW3(GLFWwindow* window, int key, int scancode, int action, int mods) {
    TwEventKeyGLFW(key, action);
}
inline void TwEventCharGLFW3(GLFWwindow* window, int codepoint) {
    TwEventCharGLFW(codepoint, GLFW_PRESS);
}
inline void TwWindowSizeGLFW3(GLFWwindow* window, int width, int height) {
    TwWindowSize(width, height);
}

void TW_CALL CLOSE(void* clientstate) {
    running = false;
}

typedef enum { WIREFRAME, CUBE, SPHERE } RenderingMode;
RenderingMode mode = WIREFRAME;
bool leafsOnly = false;
bool distanceFields = false;
bool lighting = false;
int Level = -1;

void initGUI(int width, int height, GLFWwindow* window) {

    TwInit(TW_OPENGL, nullptr);
    TwWindowSize(width, height);


    TwBar *myBar;
    myBar = TwNewBar("Main");


    TwAddButton(myBar, "Exit", CLOSE, NULL, " label='Exit' ");
    TwAddVarRW(myBar, "Leafs Only", TW_TYPE_BOOL8 ,&leafsOnly, " label='Leafs Only' ");
    TwAddVarRW(myBar, "Distance Fields", TW_TYPE_BOOL8 ,&distanceFields, " label='Distance Fields' ");
    TwAddVarRW(myBar, "Use lighting", TW_TYPE_BOOL8 ,&lighting, " label='Use lighting' ");

    TwAddVarRW(myBar, "Salir", TW_TYPE_INT32, &Level,
               " label='FPS' help='Color and transparency of the cube.' ");



    TwEnumVal modeEV[] = { {WIREFRAME, "Wireframe"}, {CUBE, "Cube"}, {SPHERE, "Sphere"} };
    TwType modeType;

// ...

// Defining season enum type
    modeType = TwDefineEnum("SeasonType", modeEV, 3);
// Adding season to bar
    TwAddVarRW(myBar, "Render Mode", modeType, &mode, NULL);


    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventMousePosGLFW3);
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventMouseWheelGLFW3);
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharGLFW3);
}
}
