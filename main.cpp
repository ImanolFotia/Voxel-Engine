#if __cplusplus < 201103L
#error This program requires at least a C++11 compliant compiler
#endif

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <include/camera.h>
#include <include/shader.h>
#include <random>
#include <ctime>
#include <include/Octree.h>
#include <include/Model.h>
#include <include/Cube.h>
#include <include/gui.h>

using namespace std;

void mouse_callback();
void Do_Movement();

static Camera camera(glm::vec3(0,0,0));

static GLfloat lastX = 400, lastY = 300;
static bool firstMouse = true;
static bool keys[1024];

static double deltaTime, lastFrame;

static float mouseScrollY = 10;
static const float width = 1280, height = 720;

static GLFWwindow* window;

static int currentLevel;
static bool currentMode;

static bool StateChanged();

int main() {
    glfwInit();
    window = glfwCreateWindow(width, height, "Sparse Voxel Octree", nullptr, nullptr);
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

    currentLevel = gui::Level = -1;
    currentMode = gui::leafsOnly = false;
    std::shared_ptr<Model> model = (std::shared_ptr<Model>) new Model("dragon.eml");

    std::shared_ptr<PointCloud> pointCloud = ( std::shared_ptr<PointCloud> ) new PointCloud( model->getVertices(), model->getNormals(), model->getIndices());

    std::shared_ptr<Octree> SVO = ( std::shared_ptr<Octree> ) new Octree( pointCloud->getPointsPositions(), pointCloud->getPointsNormals() );

    glm::mat4 projection = glm::perspective( glm::radians(75.0f), width/height, 0.1f, 3000.0f );

    TwSetCurrentWindow(0);

    Cube* cubesInstanced;
    std::vector<glm::vec3> cPositions;
    std::vector<glm::vec3> cNormals;
    std::vector<float> cScales;

    //gui::leafsOnly  =true;
    //gui::Level = 3;

    SVO->Render( cPositions, cNormals, cScales );

    cubesInstanced = new Cube(cPositions, cNormals, cScales);

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
/*
        if(currentLevel != gui::Level){

            std::cout << "State Changed" << std::endl;
            std::cout << "currentLevel " << currentLevel << std::endl;
            std::cout << "gui::Level " << gui::Level << std::endl;
            cubesInstanced->Destroy();
            delete cubesInstanced;

            cPositions.clear();
            std::vector<glm::vec3>().swap(cPositions);
            cNormals.clear();
            std::vector<glm::vec3>().swap(cNormals);
            cScales.clear();
            std::vector<float>().swap(cScales);

            SVO->Render( cPositions, cNormals, cScales );

            cubesInstanced = new Cube(cPositions, cNormals, cScales);
            currentLevel = gui::Level;
        }*/

        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &camera.GetViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniform3fv(glGetUniformLocation(shader, "viewPos"), 1, &camera.Position[0]);
        glUniform1i(glGetUniformLocation(shader, "distanceFields"), gui::distanceFields);
        cubesInstanced->Render();

        glUseProgram(0);

        TwDraw();

        glfwSwapBuffers(window);
    }

    SVO->Destroy();

    TwTerminate();
    glfwTerminate();

    cubesInstanced->Destroy();
    delete cubesInstanced;

    return 0;
}

bool StateChanged()
{
    if(currentLevel != gui::Level)
    {
        std::cout << "State Changed" << std::endl;
        return true;
    }
    else
        return false;
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
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
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
