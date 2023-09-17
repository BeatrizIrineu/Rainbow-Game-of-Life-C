#include<stdio.h>
#include<stdlib.h>
#include <pthread.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <unistd.h>

#define N 9

void color_value(float valor, float* r, float* g, float* b) {
    *r = valor;
    *g = 0.0f;
    *b = 1.0f - valor;
}

void frame_render(int x, int y, float r, float g, float b) {
    float tamanho = 2.0f / N;  // Ajustar para caber na janela de visualização
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

int get_neighbors(float** grid, int i, int j) {
    int dx[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int dy[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    int quant = 0;

    for (int k = 0; k < 8; k++) {
        int x = (i + dx[k] + N) % N; 
        int y = (j + dy[k] + N) % N; 

        if (grid[x][y] >= 1.0) {
            quant += 1;
        }
    }
    return quant;
}

float** alloc_grid(){
    float **grid  = calloc(sizeof(float*),N);
    for(int i = 0; i < N; i++){
        grid[i] = calloc(sizeof(float), N);
    }
    return grid;
}

void desalloc_grid(float** grid){
    for(int i = 0; i < N; i++){
        free(grid[i]);
    }

    free(grid);
}

float** get_new_generation(float** grid){
    float **new_grid  = alloc_grid();
    
   for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            switch(get_neighbors(grid, i, j)){
                case 2:
                    if(grid[i][j] == 1)
                        new_grid[i][j] = 1;
                    break;
                case 3:
                    new_grid[i][j] = 1;
                    break;
                default:
                    break;
                
            }
           
    }}
    
    desalloc_grid(grid);
    return new_grid;

}

int main(){
    float **grid = alloc_grid();
    

    if (!glfwInit()) {
        fprintf(stderr, "Erro ao inicializar GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Heatmap OpenGL", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Erro ao criar janela GLFW\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Erro ao inicializar GLEW\n");
        return -1;
    }

    int lin = 1, col = 1;

    grid[lin ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;

    // printf("%d", get_neighbors(grid, 1, 2));
    double nextTime = glfwGetTime() + 1;  

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();

        if (currentTime >= nextTime) {
            glClear(GL_COLOR_BUFFER_BIT);
            // Exemplo: Incrementar o valor de uma célula específica da matriz
    
            grid = get_new_generation(grid);
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    float r, g, b;
                    color_value(grid[j][i], &r, &g, &b); // Troquei a ordem de i e j aqui
                    frame_render(i, j, r, g, b);
                }
            }

            glfwSwapBuffers(window);
            nextTime += 1;

        }
            glfwPollEvents();

    }

    glfwTerminate();

    desalloc_grid(grid);
    
}
