#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define N 2048
#define NUM_THREADS 8

typedef struct {
    int start_row;
    int end_row;
    float** grid;
    float **new_grid;
} ThreadArgs;


int alives = 0;

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
    alives = 0;

    int i, j;

    #pragma omp parallel for private(j) reduction(+:alives)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            
            switch(get_neighbors(grid, i, j)){
                case 2:
                    if(grid[i][j] == 1){
                        new_grid[i][j] = 1; 
                        alives++;
                    }
                   
                    break;
                case 3:
                    new_grid[i][j] = 1;
                    alives++;
                    break;
                default:
                    new_grid[i][j] = 0;
                    break;
                
            }
        }
    }
    
    
    desalloc_grid(grid);
    return new_grid;

}

int main(){

    omp_set_num_threads(1);
    float **grid = alloc_grid();
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
    for ( i = 0; i < 2000; i++)
    {   
        grid = get_new_generation(grid);
    }

    printf("generation %d: %d\n", i, alives);
    
    gettimeofday(&end, NULL);
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Tempo gasto: %f milisegundos\n", mtime);

    desalloc_grid(grid);
    
}
