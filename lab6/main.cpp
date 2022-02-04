#include <GLFW/glfw3.h>
#include <cmath>
#include <fstream>

double turnSpeed = 5;
double scaleRatio = 1;
double alpha = 0, betta = 0, gama = 0;
int n = 50;
double h = 1, r1 = 0.5, r2 = 0.25;
double t = 0;
bool isNeg = false;
bool is_texture = true;
bool is_state;

class State {
public:
    double twining_t;
    int alpha, betta, gama;
    bool is_texture, is_neg;
};

State state;

float light_position1[] = {1, 1, 0, 0};
GLfloat light_position2[] = {0.0, 1.0, 1.0, 1.0};
GLfloat diffuse_color1[] = {1, 1, 1};
GLfloat diffuse_color2[] = {1, 1, 1};

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
        if (key == GLFW_KEY_Q && n > 3)
            --n;
        if (key == GLFW_KEY_E)
            ++n;
        if (key == GLFW_KEY_L)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        if (key == GLFW_KEY_F)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        if (key == GLFW_KEY_T)
            is_texture = !is_texture;
        if (key == GLFW_KEY_F5){
            state.is_texture = is_texture;
            state.is_neg = isNeg;
            state.alpha = alpha;
            state.betta = betta;
            state.gama = gama;
            state.twining_t = t;
            is_state = true;
        }
        if (key == GLFW_KEY_F6 && is_state){
            is_texture = state.is_texture;
            isNeg = state.is_neg;
            alpha = state.alpha;
            betta = state.betta;
            gama = state.gama;
            t = state.twining_t;
        }
    }
}

class Pointf {
public:
    float x, y, z;

    Pointf() : x(0), y(0), z(0) {

    }

    Pointf(float x, float y, float z) : x(x), y(y), z(z) {
    }
};

GLfloat M[] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        static_cast<GLfloat>(-0.5 * cos(M_PI / 6)), static_cast<GLfloat>(-0.5 * sin(M_PI / 6)), -1, 0,
        0, 0, 0, 1
};

Pointf twining_points[] = {
        Pointf(0, 0.5, 0.8),
        Pointf(0.8, 0.5, 0.8),
        Pointf(0.8, 0.5, 0),
        Pointf(0, 0.5, -0.5)
};

Pointf count_normal(Pointf p1, Pointf p2, Pointf p3) {
    Pointf a, b, n;
    GLfloat l;

    a.x = p2.x - p1.x;
    a.y = p2.y - p1.y;
    a.z = p2.z - p1.z;

    b.x = p3.x - p1.x;
    b.y = p3.y - p1.y;
    b.z = p3.z - p1.z;

    n.x = (a.y * b.z) - (a.z * b.y);
    n.y = (a.z * b.x) - (a.x * b.z);
    n.z = (a.x * b.y) - (a.y * b.x);

    // Normalize (divide by root of dot product)
    l = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    n.x /= l;
    n.y /= l;
    n.z /= l;

    return n;
}

void draw_object() {
    float a = M_PI * 2 / n;
    float x, z;


    glTranslatef(0, -h / 2, 0);

    glBegin(GL_TRIANGLE_FAN);
    {
        glNormal3f(0, -1, 0);
        glColor3f(0.7, 0.7, 0.7);
        glVertex3f(0, 0, 0);
        for (int i = -1; i < n; ++i) {
            x = sin(a * i) * r1;
            z = cos(a * i) * r1;
            glVertex3f(x, 0, z);
        }
    }
    glEnd();

    glTranslatef(0, h, 0);
    glBegin(GL_TRIANGLE_FAN);
    {
        glNormal3f(0, 1, 0);
        glColor3f(0.7, 0.7, 0.7);
        glVertex3f(0, 0, 0);
        for (int i = -1; i < n; ++i) {
            x = sin(a * i) * r2;
            z = cos(a * i) * r2;
            glVertex3f(x, 0, z);
        }
    }
    glEnd();
    glTranslatef(0, -h, 0);

    glBegin(GL_TRIANGLE_STRIP);
    {
        float x1, x2, z1, z2, x3, z3;
        glColor3f(1, 1, 1);
        for (int i = -1; i < n; ++i) {
            x1 = sin(a * i) * r1;
            z1 = cos(a * i) * r1;
            x2 = sin(a * i) * r2;
            z2 = cos(a * i) * r2;
            x3 = sin(a * (i + 1)) * r1;
            z3 = cos(a * (i + 1)) * r1;

            Pointf normal = count_normal(Pointf(x1, 0, z1), Pointf(x2, h, z2), Pointf(x3, 0, z3));
            glNormal3f(-normal.x, -normal.y, -normal.z);
            glTexCoord2d((double) (i + 1) / n, 0);
            glVertex3f(x1, 0, z1);
            glTexCoord2d((double) (i + 1) / n, 1);
            glVertex3f(x2, h, z2);
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnd();
}

Pointf twining() {
    if (t > 1)
        isNeg = true;
    if (t < 0)
        isNeg = false;
    Pointf B;
    B.x = twining_points[0].x * pow((1 - t), 3) + twining_points[1].x * 3 * pow((1 - t), 2) * t +
          twining_points[2].x * 3 * (1 - t) * t * t + twining_points[3].x * pow(t, 3);
    B.y = twining_points[0].y * pow((1 - t), 3) + twining_points[1].y * 3 * pow((1 - t), 2) * t +
          twining_points[2].y * 3 * (1 - t) * t * t + twining_points[3].y * pow(t, 3);
    B.z = twining_points[0].z * pow((1 - t), 3) + twining_points[1].z * 3 * pow((1 - t), 2) * t +
          twining_points[2].z * 3 * (1 - t) * t * t + twining_points[3].z * pow(t, 3);

    t += isNeg ? -0.005 : 0.005;
    return B;
}

GLuint loadBMP_custom(const char *imagepath) {
    // Данные, прочитанные из заголовка BMP-файла
    unsigned char header[54]; // Каждый BMP-файл начинается с заголовка, длиной в 54 байта
    unsigned int dataPos;     // Смещение данных в файле (позиция данных)
    unsigned int width, height;
    unsigned int imageSize;   // Размер изображения = Ширина * Высота * 3
// RGB-данные, полученные из файла
    unsigned char *data;

    FILE *file = fopen(imagepath, "rb");
    if (!file) {
        printf("Изображение не может быть открытоn");
        return 0;
    }

    if (fread(header, 1, 54, file) != 54) { // Если мы прочитали меньше 54 байт, значит возникла проблема
        printf("Некорректный BMP-файлn");
        return false;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Некорректный BMP-файлn");
        return 0;
    }

    // Читаем необходимые данные
    dataPos = *(int *) &(header[0x0A]); // Смещение данных изображения в файле
    imageSize = *(int *) &(header[0x22]); // Размер изображения в байтах
    width = *(int *) &(header[0x12]); // Ширина
    height = *(int *) &(header[0x16]); // Высота
    // Некоторые BMP-файлы имеют нулевые поля imageSize и dataPos, поэтому исправим их
    if (imageSize == 0) imageSize = width * height * 3; // Ширину * Высоту * 3, где 3 - 3 компоненты цвета (RGB)
    if (dataPos == 0) dataPos = 54; // В таком случае, данные будут следовать сразу за заголовком
    // Создаем буфер
    data = new unsigned char[imageSize];

// Считываем данные из файла в буфер
    fread(data, 1, imageSize, file);

// Закрываем файл, так как больше он нам не нужен
    fclose(file);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

int main() {
    glfwInit();

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(720, 720, "SCENE",
                                          nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    GLuint Texture = loadBMP_custom("/home/kurigohan933/projects/acg_labs/texture.bmp");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_TEXTURE_2D);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse_color2);

    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glTranslatef(-0.25, -0.25, 0);

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 0, 0);

        if (is_texture)
            glBindTexture(GL_TEXTURE_2D, Texture);
        else
            glBindTexture(GL_TEXTURE_2D, 0);

        glMatrixMode(GL_PROJECTION);              //проекция
        glLoadIdentity();
        glLoadMatrixf(M);

        glMatrixMode(GL_MODELVIEW);               //рисование объекта
        glLoadIdentity();
        glPushMatrix();
        glScaled(scaleRatio, scaleRatio, scaleRatio);
        glRotated(alpha, 1, 0, 0);
        glRotated(betta, 0, 1, 0);
        glRotated(gama, 0, 0, 1);
        draw_object();
        glPopMatrix();

        glBegin(GL_LINES);                       //указатели на источники
        glColor3f(1, 0, 0);
        glVertex3f(light_position1[0], light_position1[1], light_position1[2]);
        glVertex3f(0, 0, 0);
        glEnd();
//        glBegin(GL_LINES);
//        glColor3f(0, 1, 0);
//        glVertex3f(light_position2[0], light_position2[1], light_position2[2]);
//        glVertex3f(0, 0, 0);
//        glEnd();

        glPushMatrix();
        glLoadIdentity();
        Pointf B = twining();
        glTranslatef(B.x, B.y, B.z);
        float new_pos[] = {B.x, B.y, B.z, 0};
        glLightfv(GL_LIGHT1, GL_POSITION, new_pos);
        glScalef(0.2, 0.2, 0.2);
        glColor3f(0.4, 0.5, 0.4);
        glBegin(GL_TRIANGLES);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glVertex3f(1, 0, 1);
        glEnd();
        glPopMatrix();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
