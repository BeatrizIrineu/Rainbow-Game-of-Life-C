#include "graphic_functions.h"
#include <stdio.h>
#include <stdlib.h>

void color_value(float valor, float* r, float* g, float* b) {
    *r = valor;
    *g = 0.0f;
    *b = 1.0f - valor;
}

void frame_render(int x, int y, float r, float g, float b) {
    float tamanho = 2.0f / TAB;  // Ajustar para caber na janela de visualização
    float posX = x * tamanho - 1.0f;    // Ajustar para começar do canto inferior esquerdo
    float posY = 1.0f - (y + 1) * tamanho;  // Ajustar para começar do canto superior esquerdo

    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(posX, posY);
    glVertex2f(posX + tamanho, posY);
    glVertex2f(posX + tamanho, posY + tamanho);
    glVertex2f(posX, posY + tamanho);
    glEnd();
}

GLFWwindow* init_window(){

     if (!glfwInit()) {
        fprintf(stderr, "Erro ao inicializar GLFW\n");
        exit(-1);
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Heatmap OpenGL", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Erro ao criar janela GLFW\n");
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Erro ao inicializar GLEW\n");
        exit(-1);
    }

    return window;
}

void show_grid(GLFWwindow* window, float** grid, void (*f)()){
    double nextTime = glfwGetTime() + 1;  

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();

        if (currentTime >= nextTime) {
            glClear(GL_COLOR_BUFFER_BIT);
    
            f();
            for (int i = 0; i < TAB; i++) {
                for (int j = 0; j < TAB; j++) {
                    float r, g, b;
                    color_value(grid[j][i], &r, &g, &b);
                    frame_render(i, j, r, g, b);
                }
            }

            glfwSwapBuffers(window);
            nextTime += 1;

        }
            glfwPollEvents();

    }

    glfwTerminate();
}

