#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>

double turnSpeed = 5;
double scaleRatio = 1;
double alpha = 0, betta = 0, gama = 0;
int n = 5;
double h = 1, r1 = 0.5, r2 = 0.25;

struct Point {
    float x, z;

    Point(float x, float z) {
        this->x= x;
        this->z = z;
    }

    Point(){
        x = z = 0;
    }
};

std::vector<Point> o1(100), o2(100);

void calc_points(){
    float a = M_PI * 2 / n;

    float x, z;

    o1[0] = Point(0, 0);
    for (int i = 1; i < n+1; ++i) {
        x = sin(a * i) * r1;
        z = cos(a * i) * r1;
        o1[i] = Point(x, z);
    }

    o2[0] = Point(0, 0);
    for (int i = 1; i < n+1; ++i) {
        x = sin(a * i) * r2;
        z = cos(a * i) * r2;
        o2[i] = Point(x, z);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (yoffset > 0)
        scaleRatio += 0.1;
    else if (scaleRatio > 0.2)
        scaleRatio -= 0.1;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_REPEAT)
        switch (key) {
            case GLFW_KEY_LEFT:
                gama -= turnSpeed;
                break;
            case GLFW_KEY_RIGHT:
                gama += turnSpeed;
                break;
            case GLFW_KEY_DOWN:
                alpha += turnSpeed;
                break;
            case GLFW_KEY_UP:
                alpha -= turnSpeed;
                break;
            case GLFW_KEY_DELETE:
                betta -= turnSpeed;
                break;
            case GLFW_KEY_PAGE_DOWN:
                betta += turnSpeed;
                break;
            case GLFW_KEY_Q:
                if (n > 3)
                    --n;
                break;
            case GLFW_KEY_E:
                ++n;
                break;
        }
    else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q && n > 3) {
            --n;
            calc_points();
        }
        if (key == GLFW_KEY_E && n < 100) {
            ++n;
            calc_points();
        }
        if (key == GLFW_KEY_L)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (key == GLFW_KEY_F)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


struct Pointf {
    float x;
    float y;
    float z;
};

double M[] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        -0.5 * cos(M_PI / 6), -0.5 * sin(M_PI / 6), -1, 0,
        0, 0, 0, 1
};

Pointf vertices[] = {{-0.25, -0.25, -0.25},
                     {0.25,  -0.25, -0.25},
                     {0.25,  0.25,  -0.25},
                     {-0.25, 0.25,  -0.25},
                     {-0.25, -0.25, 0.25},
                     {0.25,  -0.25, 0.25},
                     {0.25,  0.25,  0.25},
                     {-0.25, 0.25,  0.25}};

float colors[] = {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
GLuint indexes[] = {0, 1, 2, 3, 0, 4, 7, 3, 0, 1, 5, 4, 6, 2, 1, 5, 6, 7, 4, 5, 6, 7, 3, 2
};

void draw_object() {
    float x, z;

    glPushMatrix();

    glTranslatef(0, -h / 2, 0);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.2, 0, 0.7);
    {
        for (int i = 0; i<n+1; ++i)
            glVertex3f(o1[i].x, 0, o1[i].z);
        glVertex3f(o1[1].x, 0, o1[1].z);
    }
    glEnd();

    glTranslatef(0, h, 0);
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.2, 0, 0.7);
    {
        for (int i = 0; i<n+1; ++i)
            glVertex3f(o2[i].x, 0, o2[i].z);
        glVertex3f(o2[1].x, 0, o2[1].z);
    }
    glEnd();
    glTranslatef(0, -h, 0);

    glBegin(GL_TRIANGLE_STRIP);
    {
        glColor3f(0, 0.3, 0.5);
        for (int i = 1; i < n+1; ++i) {
            glVertex3f(o1[i].x, 0, o1[i].z);
            glVertex3f(o2[i].x, h, o2[i].z);
        }
        glVertex3f(o1[1].x, 0, o1[1].z);
        glVertex3f(o2[1].x, h, o2[1].z);
    }
    glEnd();
    glPopMatrix();
}

int main() {
    calc_points();

    glfwInit();

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(720, 720, "SCENE",
                                          nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glTranslatef(-0.25, -0.25, 0);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1, 1, 1, 0);

        glVertexPointer(3, GL_FLOAT, 0, &vertices);
        glColorPointer(3, GL_FLOAT, 0, &colors);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);


        glLoadIdentity();
        glScaled(scaleRatio, scaleRatio, 1);
        glMultMatrixd(M);
        glRotated(alpha, 1, 0, 0);
        glRotated(betta, 0, 1, 0);
        glRotated(gama, 0, 0, 1);
        draw_object();

        glLoadIdentity();
        glTranslatef(-0.75, -0.75, 0);
        glScalef(0.5, 0.5, 1);
        glMultMatrixd(M);
        glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, &indexes);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
