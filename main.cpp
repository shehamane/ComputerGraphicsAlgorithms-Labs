#include <GLFW/glfw3.h>

struct Pointf {
    float x;
    float y;
    float z;
};

Pointf vertices[] = {{0, 0, 0},
                     {0.5, 0, 0},
                     {0.5, 0.5, 0},
                     {0, 0.5, 0},
                     {0, 0, 0.5},
                     {0.5, 0, 0.5},
                     {0.5, 0.5, 0.5},
                     {0, 0.5, 0.5}};

float colors[] = {1,0,0, 0,1,0, 0,0,1, 1,0,0, 0,1,0, 0,0,1, 0.5,0.5,0.5, 1,1,1};
GLuint indexes[] = {0, 1, 2, 3, 0, 1, 5, 4, 0, 3, 7, 4, 4, 5, 6, 7, 6, 5, 1, 2, 6, 7, 3, 2};

int main() {
    glfwInit();

    glfwDefaultWindowHints();
    GLFWwindow *window = glfwCreateWindow(720, 720, "SCENE",
                                          nullptr, nullptr);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwMakeContextCurrent(window);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 0, 2);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0.5, 0.5, 0);

//        glRotatef(1, 1, 1, 0);

        glVertexPointer(3, GL_FLOAT, 0, &vertices);
        glColorPointer(3, GL_FLOAT, 0, &colors);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, &indexes);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
