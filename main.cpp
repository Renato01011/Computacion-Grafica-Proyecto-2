#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "Camera.h"
#include "Objects.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float initTime = 0.0f;
float currTime = 0.0f;

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
GLuint luna_vao;
int luna_numIndices;

// Objets
std::vector<Object*> vObj;

// Ball
Model mball(glm::vec3(0.0f, -5.0f, 15.0f), 0.0, glm::vec3(0.1f, 0.1f, 0.1f), "./Models/Ball/ball.obj", "ball");   

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // Configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("./VertexShader.vs", "./FragmentShader.fs");

    // Sphere For Testing
    //Sphere s1(glm::vec3(0.0f, 0.0f, 0.0f), 0.0, glm::vec3(1.0f, 1.0f, 1.0f), 4.0f, 100, 100);
    //s1.pos_ini = glm::vec3(0.0f,0.0f,0.0f); s1.vel_ini = glm::vec3(10,10,10); s1.ang_ini = 45.0f;
    //s1.Setup();
    //vObj.emplace_back(&s1);

    // Level 1 Boxes
    Model mbox(glm::vec3(0.0f, 0.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    mbox.Setup();
    vObj.emplace_back(&mbox);
    Model mbox1(glm::vec3(-5.0f, 0.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox1.Setup();
    mbox1.m = mbox.m;
    mbox1.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox1);
    Model mbox2(glm::vec3(5.0f, 0.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox2.Setup();
    mbox2.m = mbox.m;
    mbox2.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox2);
    Model mbox3(glm::vec3(0.0f, 5.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox3.Setup();
    mbox3.m = mbox.m;
    mbox3.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox3);
    Model mbox4(glm::vec3(0.0f, -5.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox4.Setup();
    mbox4.m = mbox.m;
    mbox4.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox4);
    Model mbox5(glm::vec3(5.0f, -5.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox5.Setup();
    mbox5.m = mbox.m;
    mbox5.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox5);
    Model mbox6(glm::vec3(-5.0f, -5.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox6.Setup();
    mbox6.m = mbox.m;
    mbox6.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox6);
    Model mbox7(glm::vec3(5.0f, 5.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox7.Setup();
    mbox7.m = mbox.m;
    mbox7.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox7);
    Model mbox8(glm::vec3(-5.0f, 5.0f, -7.0f), 0.0, glm::vec3(0.6f, 0.6f, 0.6f), "./Models/Box/box.obj", "box");
    //mbox8.Setup();
    mbox8.m = mbox.m;
    mbox8.textures_loaded = mbox.textures_loaded;
    vObj.emplace_back(&mbox8);
        
    // Setup Ball
    mball.Setup();

    // Background
    Model mfloor(glm::vec3(0.0, -7.0f, 0.0f), 0.0, glm::vec3(7.0f, 7.0f, 7.0f), "./Models/Floor/floor.obj", "floor");
    mfloor.Setup();
    vObj.emplace_back(&mfloor);

    Model mWall(glm::vec3(0.0f,-6.0f,-15.0f), 0.0f, glm::vec3(0.1f,0.1f,0.1f), "./Models/Wall/wall.obj", "wall");
    mWall.Setup();
    vObj.emplace_back(&mWall);
    Model mWall1(glm::vec3(-13.0f, -6.0f, -7.0f), 90.0, glm::vec3(0.1f,0.1f,0.1f), "./Models/Box/box.obj", "wall");
    //mWall1.Setup();
    mWall1.m = mWall.m;
    mWall1.textures_loaded = mWall.textures_loaded;
    vObj.emplace_back(&mWall1);
    Model mWall2(glm::vec3(13.0f, -6.0f, -7.0f), -90.0, glm::vec3(0.1f,0.1f,0.1f), "./Models/Box/box.obj", "wall");
    //mWall2.Setup();
    mWall2.m = mWall.m;
    mWall2.textures_loaded = mWall.textures_loaded;
    vObj.emplace_back(&mWall2);
    Model mWall3(glm::vec3(13.0f, -6.0f, 10.0f), -90.0, glm::vec3(0.1f,0.1f,0.1f), "./Models/Box/box.obj", "wall");
    //mWall3.Setup();
    mWall3.m = mWall.m;
    mWall3.textures_loaded = mWall.textures_loaded;
    vObj.emplace_back(&mWall3);
    Model mWall4(glm::vec3(-13.0f, -6.0f, 10.0f), 90.0, glm::vec3(0.1f,0.1f,0.1f), "./Models/Box/box.obj", "wall");
    //mWall4.Setup();
    mWall4.m = mWall.m;
    mWall4.textures_loaded = mWall.textures_loaded;
    vObj.emplace_back(&mWall4);
    Model mWall5(glm::vec3(0.0f, -6.0f, 22.0f), 180.0, glm::vec3(0.1f,0.1f,0.1f), "./Models/Box/box.obj", "wall");
    //mWall5.Setup();
    mWall5.m = mWall.m;
    mWall5.textures_loaded = mWall.textures_loaded;
    vObj.emplace_back(&mWall5);
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        currTime = currentFrame - initTime;

        processInput(window);
        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("objectColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("lightPos", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);

        // View/Projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        // Draw Objects
        for (auto obj : vObj) {
            obj->Update(currTime);
            obj->Draw(lightingShader);
        }

        mball.Update(currTime);
        mball.CollisionDetection(vObj);
        mball.Draw(lightingShader);

        if (CollidedObject != nullptr) {
            for (auto itr = vObj.begin(); itr != vObj.end(); itr++) {
                if (*itr == CollidedObject) {
                    vObj.erase(itr);
                    CollidedObject = nullptr;
                    MoveBall = false;
                    mball.pos = glm::vec3(0.0f, -5.0f, 15.0f);
                    break;
                }
            }
        }

        if (mball.pos.y < -10.0f || mball.pos.z < -15.0f || mball.pos.x < -13.0f || mball.pos.x > 13.0f) {
            mball.pos = glm::vec3(0.0f, -5.0f, 15.0f);
            MoveBall = false;
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, luna_vao);
    //glDeleteVertexArrays(1, &lightCubeVAO);
    //glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        if (!MoveBall) {
            initTime = static_cast<float>(glfwGetTime());
            MoveBall = true;
            mball.pos_ini = glm::vec3(0.0f,-5.0f,15.0f); mball.vel_ini = glm::vec3(camera.Front.x*50,camera.Front.y*50,-50);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE){
        
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Make sure the viewport matches the new window dimensions; note that width and
    // Height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
};