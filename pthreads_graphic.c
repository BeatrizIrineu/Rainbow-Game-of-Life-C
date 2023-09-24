#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "graphic_functions.h"

#define N 2048
#define NUM_THREADS 8

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int start_row;
    int end_row;
    float **grid;
    float **new_grid;
} ThreadArgs;

pthread_t threads[NUM_THREADS];
ThreadArgs targs[NUM_THREADS]; 

float **temp;
float **grid;
float **new_grid;


int alives = 0;
int generation = 0;

//http://www.each.usp.br/digiampietri/ed/aula11.pdf
//utilizamos o conceito de fila circular para iterar sobre as bordas da matriz
int get_neighbors(float** grid, int i, int j) {
    int x_row[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int y_column[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    int quant = 0;

    for (int k = 0; k < 8; k++) {
        int x = (i + x_row[k] + N) % N; 
        int y = (j + y_column[k] + N) % N; 

        if (grid[x][y] > 0) {
            quant += 1;
        }
    }
    return quant;
}

float mean_from_neighbors(float **grid, int i, int j){
    int x_row[] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int y_column[] = {-1,  0,  1, -1, 1, -1, 0, 1};
    float soma = 0;

    for (int k = 0; k < 8; k++) {
        int x = (i + x_row[k] + N) % N; 
        int y = (j + y_column[k] + N) % N; 

        soma += grid[x][y];
    }
    return soma/8;
}

void* process_matrix(void* args) {

    ThreadArgs* targs = (ThreadArgs*)args;
    for (int i = targs->start_row; i < targs->end_row; i++) {
        for (int j = 0; j < N; j++) {
            
            switch(get_neighbors(targs->grid, i, j)){
                case 2:
                    if(targs->grid[i][j] > 0){
                        targs->new_grid[i][j] =  mean_from_neighbors(grid, i, j) > 0 ? 1.0 : 0;
                        pthread_mutex_lock(&count_mutex); // Bloqueia o mutex
                        alives++;
                        pthread_mutex_unlock(&count_mutex); // Desbloqueia o mutex
                    }else targs->new_grid[i][j] = 0;
                    break;
                case 3:
                    targs->new_grid[i][j] =  mean_from_neighbors(grid, i, j) > 0 ? 1.0 : 0;
                    pthread_mutex_lock(&count_mutex); // Bloqueia o mutex
                    alives++;
                    pthread_mutex_unlock(&count_mutex); // Desbloqueia o mutex
                    break;
                default:
                    targs->new_grid[i][j] = 0;
                    break;
                
            }
        }
    }

    return NULL;
}


float** alloc_grid(){
    float **grid  = (float**) calloc(sizeof(float*),N);
    for(int i = 0; i < N; i++){
        grid[i] =  (float*) calloc(sizeof(float), N);
    }
    return grid;
}

void desalloc_grid(float** grid){
    for(int i = 0; i < N; i++){
        free(grid[i]);
    }

    free(grid);
}

void get_new_generation(){
    alives = 0;
    int rows_per_thread = N / NUM_THREADS;

     for (int i = 0; i < NUM_THREADS; i++) {
        targs[i].start_row = i * rows_per_thread;
        targs[i].end_row = (i + 1) * rows_per_thread;
        targs[i].new_grid = new_grid;
        targs[i].grid = grid;
        pthread_create(&threads[i], NULL, process_matrix, &targs[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    temp = grid;
    grid = new_grid;
    new_grid = temp;
    printf("generation %d: %d\n", generation, alives);
}

int main(){

    grid = alloc_grid();
    new_grid  = alloc_grid();

    struct timeval start, end;
    long seconds, useconds;
    double mtime;
    int lin = 1, col = 1;

    grid[lin ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;


    lin =10; col = 30;
    grid[lin  ][col+1] = 1.0;
    grid[lin  ][col+2] = 1.0;
    grid[lin+1][col  ] = 1.0;
    grid[lin+1][col+1] = 1.0;
    grid[lin+2][col+1] = 1.0;

    gettimeofday(&start, NULL);
    int i;

    GLFWwindow* window = init_window();

    // for ( i = 0; i < 2000; i++)
    // {   
    //     alives = 0;
    //     get_new_generation();
       
    // }

    show_grid(window, grid, get_new_generation);

    gettimeofday(&end, NULL);
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Tempo gasto: %f milisegundos\n", mtime);

    desalloc_grid(grid);
    desalloc_grid(new_grid);

}
