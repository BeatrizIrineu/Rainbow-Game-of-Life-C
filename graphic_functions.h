#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define TAB 50

void color_value(float valor, float* r, float* g, float* b);
void frame_render(int x, int y, float r, float g, float b);
GLFWwindow* init_window();
void show_grid(GLFWwindow* window, float** grid, void (*f)());