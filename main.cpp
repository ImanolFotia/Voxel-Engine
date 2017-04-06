#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <camera.h>
#include <Node.h>
#include <random>
#include <ctime>

using namespace std;

void mouse_callback();
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
float mouseScrollY = 10;
void Do_Movement();
GLFWwindow* window;
double deltaTime, lastFrame;
Camera camera(glm::vec3(0,0,0));
float width = 1280, height = 720;
int main()
{
    glfwInit();
    window = glfwCreateWindow(width, height, "Octree", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint shader = crearShader("vertex.glsl", "fragment.glsl");

    std::vector<glm::vec3> ps;

    srand(time(0));

    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
    std::default_random_engine generator;

    for(int i = 0; i < 10; ++i)
        for(int j = 0; j < 10; ++j)
            for(int k = 0; k < 10; ++k)
                ps.push_back(glm::vec3(randomFloats(generator), randomFloats(generator), randomFloats(generator)));

    glPointSize(5.0);
    glfwSetCursorPos(window, 0.5, 0.5);
    std::shared_ptr<OctreeNode> RootNode = (std::shared_ptr<OctreeNode>) new OctreeNode(glm::vec3(0.0), 5.0, ps, 0);
    glm::vec3 point = glm::vec3(0,0,0);
    glm::mat4 projection = glm::perspective(75.0f, width/height, 0.1f, 1000.0f);
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3), &point, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glLineWidth(3.0);
    while(!glfwWindowShouldClose(window))
    {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        Do_Movement();

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(VAO);
        for(int i = 0; i < ps.size(); ++i){
                glm::mat4 model = glm::translate(glm::mat4(), ps[i]);
        glm::mat4 MVP = projection * camera.GetViewMatrix() * model;
        glUniformMatrix4fv(glGetUniformLocation(shader, "MVP"), 1, GL_FALSE, &MVP[0][0]);

        glDrawArrays(GL_POINTS, 0, 1);
        }
        glUseProgram(0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void Do_Movement()
{
    // Camera controls
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    mouse_callback();
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void mouse_callback()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    //if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    camera.ProcessMouseMovement(-xoffset, -yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
