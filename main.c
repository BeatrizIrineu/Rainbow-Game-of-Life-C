#include<stdio.h>
#include<stdlib.h>
#define N 2048

//retorne a quantidade de vizinhos vivos de cada célula na posição i,j:
int get_neighbors(float** grid, int i, int j) {}

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

int main(){
    
    float **grid = alloc_grid();
    float **new_grid  = alloc_grid();

    grid[0][1] = 2.4;
    printf("%f", grid[0][1]);

    desalloc_grid(grid);
    desalloc_grid(new_grid);
}