#if __cplusplus < 201103L
#error This program requires at least a C++11 compliant compiler
#endif

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <include/camera.h>
#include <include/shader.h>
#include <include/Node.h>
#include <random>
#include <ctime>
#include <include/Octree.h>
#include <include/PointCloud.h>
#include <include/Model.h>
#include <include/gui.h>

using namespace std;

void mouse_callback();
void Do_Movement();

Camera camera(glm::vec3(0,0,0));

GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
bool keys[1024];

double deltaTime, lastFrame;

float mouseScrollY = 10;
const float width = 1280, height = 720;

GLFWwindow* window;



int main() {
    glfwInit();
    window = glfwCreateWindow(width, height, "Sparse Voxel Octree", 0, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glLineWidth(2.0);
    glPointSize(10.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    GLuint shader = crearShader( "vertex.glsl", "fragment.glsl" );

    gui::initGUI(width, height, window);

    std::shared_ptr<Model> model = (std::shared_ptr<Model>) new Model("bunny.eml");

    std::shared_ptr<PointCloud> pointCloud = ( std::shared_ptr<PointCloud> ) new PointCloud( model->getVertices(), model->getNormals());

    std::shared_ptr<Octree> SVO = ( std::shared_ptr<Octree> ) new Octree( pointCloud->getPointsPositions(), pointCloud->getPointsNormals() );

    glm::mat4 projection = glm::perspective( glm::radians(75.0f), width/height, 0.1f, 3000.0f );


    TwSetCurrentWindow(0);

    while( gui::running ) {

        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        Do_Movement();

        glfwPollEvents();
        camPos = camera.Position;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        if(gui::distanceFields){
           glEnable(GL_BLEND);

            glDisable(GL_DEPTH_TEST);
        }
        else{
           glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }


        if( false ) {
            pointCloud->Render(shader, projection, camera.GetViewMatrix());
        }

        SVO->Render( shader, projection, camera.GetViewMatrix() );

        glUseProgram(0);

        TwDraw();

        glfwSwapBuffers(window);
    }

    SVO->Destroy();

    TwTerminate();
    glfwTerminate();

    return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement() {
    // Camera controls
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    mouse_callback();
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024) {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback() {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if(firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    //if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    camera.ProcessMouseMovement(xoffset, yoffset, window);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
