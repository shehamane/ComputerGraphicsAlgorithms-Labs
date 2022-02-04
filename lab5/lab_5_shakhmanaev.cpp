#include <GLFW/glfw3.h>
#include <vector>

float rotationX = 0, rotationY = 0, rotationZ = 0;
bool isClip = false;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_UP:
                rotationX += 5;
                break;
            case GLFW_KEY_DOWN:
                rotationX -= 5;
                break;
            case GLFW_KEY_RIGHT:
                rotationY -= 5;
                break;
            case GLFW_KEY_LEFT:
                rotationY += 5;
                break;
            case GLFW_KEY_A:
                rotationZ += 5;
                break;
            case GLFW_KEY_D:
                rotationZ -= 5;
                break;
            case GLFW_KEY_F:
                isClip = !isClip;
                break;
            default:
                break;
        }
    }
}

class Point {
public:
    float x, y, z;

    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Line {
public:
    Point v0, v1;

    Line(Point v0, Point v1) : v0(v0), v1(v1) {}
};

class Vector3f {
public:
    float x, y, z;

    Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3f() : x(0), y(0), z(0) {}

    Vector3f(Point a, Point b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}
};

float scalarProduct(Vector3f u, Vector3f v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

class Triangle {
public:
    Point a, b, c;
    Vector3f n;

    Triangle(Point a, Point b, Point c, Vector3f n) : a(a), b(b), c(c), n(n) {}
};

void drawLine(Point a, Point b) {
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(a.x, a.y, a.z);
    glVertex3f(b.x, b.y, b.z);

    glEnd();
}

int main() {
    int width = 1024, height = 1024;

    std::vector<Triangle> triangles;
    triangles.emplace_back(Point(0.7, 0, 0), Point(0, 0.7, 0), Point(0, 0, 0), Vector3f(0, 0, -1));
    triangles.emplace_back(Point(0.7, 0, 0), Point(0, 0, 0.7), Point(0, 0, 0), Vector3f(0, -1, 0));
    triangles.emplace_back(Point(0, 0.7, 0), Point(0, 0, 0.7), Point(0, 0, 0), Vector3f(-1, 0, 0));
    triangles.emplace_back(Point(0.7, 0, 0), Point(0, 0, 0.7), Point(0, 0.7, 0), Vector3f(1, 1, 1));

    Line line(Point(0.2, 0.1, 1), Point(0.1, 0.2, -1));

    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, "SCENE",
                                          nullptr, nullptr);
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLoadIdentity();

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glPushMatrix();

        glRotatef(rotationX, 1, 0, 0);
        glRotatef(rotationY, 0, 1, 0);
        glRotatef(rotationZ, 0, 0, 1);

        glBegin(GL_TRIANGLES);
        glColor3f(1, 1, 1);
        for (Triangle triangle : triangles) {
            glVertex3f(triangle.a.x, triangle.a.y, triangle.a.z);
            glVertex3f(triangle.b.x, triangle.b.y, triangle.b.z);
            glVertex3f(triangle.c.x, triangle.c.y, triangle.c.z);
        }
        glEnd();
        if (!isClip) {
            glBegin(GL_LINES);

            glColor3f(1, 0, 0);
            glVertex3f(line.v0.x, line.v0.y, line.v0.z);
            glVertex3f(line.v1.x, line.v1.y, line.v1.z);

            glEnd();
        } else {
            Vector3f d = Vector3f(line.v0, line.v1);
            float t0 = 0, t1 = 1;
            for (Triangle triangle : triangles) {
                Vector3f w = Vector3f(triangle.a, line.v0);

                float P = scalarProduct(triangle.n, d);
                float Q = scalarProduct(triangle.n, w);

                if (P == 0 && Q > 0) {
                    drawLine(line.v0, line.v1);
                    break;
                }
                if (P != 0) {
                    float t = -Q / P;
                    if (t < 0 || t > 1)
                        continue;
                    else if (P > 0) {
                        if (t <= t0)
                            continue;
                        else if (t < t1)
                            t1 = t;
                    } else if (P < 0) {
                        if (t >= t1)
                            continue;
                        else if (t > t0)
                            t0 = t;
                    }
                }
            }
            float x1 = line.v0.x + (line.v1.x - line.v0.x) * t0;
            float y1 = line.v0.y + (line.v1.y - line.v0.y) * t0;
            float z1 = line.v0.z + (line.v1.z - line.v0.z) * t0;

            float x2 = line.v0.x + (line.v1.x - line.v0.x) * t1;
            float y2 = line.v0.y + (line.v1.y - line.v0.y) * t1;
            float z2 = line.v0.z + (line.v1.z - line.v0.z) * t1;

            drawLine(line.v0, Point(x1, y1, z1));
            drawLine(line.v1, Point(x2, y2, z2));

        }
        glPopMatrix();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
