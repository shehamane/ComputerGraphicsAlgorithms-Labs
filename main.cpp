#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

#define STANDBY 0
#define VERTEX_CREATING 1
#define WITH_POSTFILTRATION 2
#define WITHOUT_POSTFILTRATION 3

struct Edge {
    int x1, y1;
    int x2, y2;
    bool spec;

    Edge(int x1, int y1, int x2, int y2) {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
        spec = false;
    }

    Edge(int x, int y) {
        x1 = x;
        y1 = y;
        x2 = y2 = -1;
        spec = false;
    }

    Edge() = default;
};

struct Data {
    std::vector<Edge> edges;
    float **frame_buffer;
    char state;
    int width, height;

    Data() {
        this->state = STANDBY;
    }
};

void clearFrame(float *frame_buffer, int width, int height) {
    for (int i = 0; i < width * height * 3; ++i)
        frame_buffer[i] = 0;
}


void makePostFiltration(float *frame_buffer, const std::vector<Edge> &edges, int width, int height);

int max(int x, int y) {
    return x > y ? x : y;
}

int min(int x, int y) {
    return x < y ? x : y;
}

int Sign(int x) {
    if (x > 0)
        return 1;
    else if (x == 0)
        return 0;
    else
        return -1;
}

int getPixelIndex(int x, int y, int width, int height) {
    return 3 * ((height - y) * width + x);
}

int getPixelIndex2(int x, int y, int width) {
    return 3 * (y * width + x);
}

void reversePixel(float *frame_buffer, int index) {
    if (frame_buffer[index] == 0)
        frame_buffer[index] = frame_buffer[index + 1] = frame_buffer[index + 2] = 1;
    else
        frame_buffer[index] = frame_buffer[index + 1] = frame_buffer[index + 2] = 0;
}

void fillRowBresencham(float *frame_buffer, Edge edge, int x_baffle, int width, int height) {
    int x = edge.x1, y = edge.y1;
    int Dx = abs(edge.x2 - edge.x1), Dy = abs(edge.y2 - edge.y1);
    int sx = Sign(edge.x2 - edge.x1), sy = Sign(edge.y2 - edge.y1);
    bool is_swap = false;

//    std::cout << edge.x1 << " " << edge.y1 << " --> " << edge.x2 << " " << edge.y2 << std::endl;
//    std::cout << "Dx = " << Dx << ", Dy = " << Dy << std::endl;

    if (Dy < Dx) {
        int tmp = Dx;
        Dx = Dy;
        Dy = tmp;
        is_swap = true;
    }

    int e = 2 * Dy - Dx;

    if (!edge.spec)
        for (int j = min(x, x_baffle); j <= max(x, x_baffle); ++j) {
            if (x > x_baffle && j == x_baffle)
                continue;
            reversePixel(frame_buffer, getPixelIndex(j, y, width, height));
        }
    for (int i = 0; i < Dx; ++i) {
        while (e >= 0) {
            if (is_swap)
                x += sx;
            else
                y += sy;
            e -= 2 * Dx;

            if (!is_swap)
                for (int j = min(x, x_baffle); j <= max(x, x_baffle); ++j) {
                    if (x > x_baffle && j == x_baffle)
                        continue;
                    reversePixel(frame_buffer, getPixelIndex(j, y, width, height));
                }
        }
        if (is_swap)
            y += sy;
        else
            x += sx;
        e += 2 * Dy;

        if (is_swap)
            for (int j = min(x, x_baffle); j <= max(x, x_baffle); ++j) {
                if (x > x_baffle && j == x_baffle)
                    continue;
                reversePixel(frame_buffer, getPixelIndex(j, y, width, height));
            }
    }
}

void setBaffleColor(float *frame_buffer, int x_baffle, int width, int height) {
    for (int i = 0; i < height - 1; ++i) {
        int index = getPixelIndex2(x_baffle, i, width);
        frame_buffer[index] = 1;
        frame_buffer[index + 1] = 0;
        frame_buffer[index + 2] = 0;
    }
}

void fillPolygonBaffle(float *frame_buffer, const std::vector<Edge> &edges, int width, int height) {
    int x_min = 9999, x_max = -1;
    for (Edge edge : edges) {
        if (edge.x1 < x_min)
            x_min = edge.x1;
        if (edge.x1 > x_max)
            x_max = edge.x1;
    }
    int x_baffle = ceil((x_max + x_min) / 2);

    for (Edge edge : edges)
        fillRowBresencham(frame_buffer, edge, x_baffle, width, height);
    setBaffleColor(frame_buffer, x_baffle, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Data *data = static_cast<Data * >(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        if (data->state == VERTEX_CREATING)
            data->state = WITHOUT_POSTFILTRATION;
        else if (data->state == WITHOUT_POSTFILTRATION) {
            data->state = WITH_POSTFILTRATION;
            makePostFiltration(*data->frame_buffer, data->edges, data->width, data->height);
        } else if (data->state == WITH_POSTFILTRATION) {
            data->state = WITHOUT_POSTFILTRATION;
            clearFrame(*data->frame_buffer, data->width, data->height);
            fillPolygonBaffle(*data->frame_buffer, data->edges, data->width, data->height);
        }
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        data->state = STANDBY;
        data->edges.clear();
        clearFrame(*data->frame_buffer, data->width, data->height);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    Data *data = static_cast<Data * >(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS &&
        (data->state == STANDBY || data->state == VERTEX_CREATING)) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        data->edges.emplace_back(Edge((int) x, (int) y));

        data->edges[data->edges.size() - 1].x2 = data->edges[0].x1;
        data->edges[data->edges.size() - 1].y2 = data->edges[0].y1;
        for (int i = 1; i < (int) data->edges.size(); ++i) {
            data->edges[i - 1].x2 = data->edges[i].x1;
            data->edges[i - 1].y2 = data->edges[i].y1;
        }

        if ((data->edges[0].y1 >= data->edges[data->edges.size() - 1].y1 &&
             data->edges[0].y1 <= data->edges[0].y2) ||
            (data->edges[0].y1 <= data->edges[data->edges.size() - 1].y1 &&
             data->edges[0].y1 >= data->edges[0].y2))
            data->edges[0].spec = true;
        else
            data->edges[0].spec = false;
        for (int i = 1; i < (int) data->edges.size(); ++i)
            if ((data->edges[i].y1 >= data->edges[i - 1].y1 &&
                 data->edges[i].y1 <= data->edges[i].y2) ||
                (data->edges[i].y1 <= data->edges[i - 1].y1 &&
                 data->edges[i].y1 >= data->edges[i].y2))
                data->edges[i].spec = true;
            else
                data->edges[i].spec = false;

        if (data->edges.size() == 3)
            data->state = VERTEX_CREATING;

        clearFrame(*data->frame_buffer, data->width, data->height);
        fillPolygonBaffle(*data->frame_buffer, data->edges, data->width, data->height);
    }
}


int main() {
    int width = 1024, height = 1024;
    auto *frame_buffer = new float[width * height * 3];
    std::vector<Edge> edges;

    Data data;
    data.frame_buffer = &frame_buffer;
    data.width = width;
    data.height = height;

    glfwInit();

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(width, height, "SCENE",
                                          nullptr, nullptr);
    glfwSetWindowUserPointer(window, &data);
    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    glLoadIdentity();

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGB, GL_FLOAT, frame_buffer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete[] data.frame_buffer;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


void makePostFiltration(float *frame_buffer, const std::vector<Edge> &edges, int width, int height) {
    clearFrame(frame_buffer, width, height);
    int new_width = width * 3, new_height = height * 3;
    auto *new_frame_buffer = new float[new_width * new_height * 3];

    std::vector<Edge> new_edges;
    for (Edge edge : edges) {
        new_edges.push_back(edge);
        new_edges.back().y1 *= 3;
        new_edges.back().x1 *= 3;
        new_edges.back().x2 *= 3;
        new_edges.back().y2 *= 3;
    }

    fillPolygonBaffle(new_frame_buffer, new_edges, new_width, new_height);

    for (int i = 0; i < height - 1; ++i)
        for (int j = 0; j < width - 1; ++j) {
            int index = getPixelIndex2(j, i, width);
            for (int q = 0; q < 3; ++q)
                for (int u = 0; u < 3; ++u) {
                    int nei_index = getPixelIndex2(j * 3 + u, i * 3 + q, new_width);
                    frame_buffer[index] += new_frame_buffer[nei_index];
                    frame_buffer[index + 1] += new_frame_buffer[nei_index + 1];
                    frame_buffer[index + 2] += new_frame_buffer[nei_index + 2];
                }
            frame_buffer[index] /= 9;
            frame_buffer[index + 1] /= 9;
            frame_buffer[index + 2] /= 9;
        }

    delete[] new_frame_buffer;
}

//void makePostFiltration(float **frame_buffer, int width, int height) {
//    for (int i = 0; i < height-1; ++i)
//        for (int j = 0; j < width-1; ++j) {
//            int n = 1;
//            int index = getPixelIndex2(j, i, width);
//            float r = (*frame_buffer)[index], g = (*frame_buffer)[index + 1], b = (*frame_buffer)[index + 2];
////            std::cout << r << " " << g << " " << b << std:: endl;
//            if (i > 0) {
//                ++n;
//                int nei_index = getPixelIndex2(j, i - 1, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (i < height + 1) {
//                ++n;
//                int nei_index = getPixelIndex2(j, i + 1, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (j > 0) {
//                ++n;
//                int nei_index = getPixelIndex2(j - 1, i, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (j < width + 1) {
//                ++n;
//                int nei_index = getPixelIndex2(j + 1, i, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (i > 0 && j > 0) {
//                ++n;
//                int nei_index = getPixelIndex2(j - 1, i - 1, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (i > 0 && j < width + 1) {
//                ++n;
//                int nei_index = getPixelIndex2(j + 1, i - 1, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (i < height + 1 && j > 0) {
//                ++n;
//                int nei_index = getPixelIndex2(j - 1, i + 1, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            if (i < height + 1 && j < width + 1) {
//                ++n;
//                int nei_index = getPixelIndex2(j + 1, i + 1, width);
//                r += (*frame_buffer)[nei_index];
//                g += (*frame_buffer)[nei_index + 1];
//                b += (*frame_buffer)[nei_index + 2];
//            }
//            (*frame_buffer)[index] = r / n;
//            (*frame_buffer)[index + 1] = g / n;
//            (*frame_buffer)[index + 2] = b / n;
////            std::cout << (*frame_buffer)[index] << " " << (*frame_buffer)[index + 1] << " " << (*frame_buffer)[index + 2] << std:: endl;
//        }
//}