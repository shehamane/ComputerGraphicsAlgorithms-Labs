#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

int main() {
    if(!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwDefaultWindowHints();

    GLFWwindow* window = glfwCreateWindow(300, 300, "Red Triangle",
                                          nullptr, nullptr);
    if(window == nullptr) {
        std::cerr << "Failed to open GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwShowWindow(window);

    // Важно! Не эквивалентно glEnable(GL_DEPTH_TEST | GL_DOUBLEBUFFER)
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DOUBLEBUFFER);
    glDepthFunc(GL_LESS);

    glClearColor(0, 0, 0, 1);

    glm::mat4 m = glm::perspective(45.0f, 4.0f / 3.0f, 1.0f, 100.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(m));

    while(glfwWindowShouldClose(window) == GL_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glColor4f(1, 0, 0, 1);
        glBegin(GL_TRIANGLES);
        glVertex3f(   0,  0.5, -5);
        glVertex3f( 0.5, -0.5, -5);
        glVertex3f(-0.5, -0.5, -5);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
