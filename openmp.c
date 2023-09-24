#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define N 2048

float **grid;
float **new_grid;
float **temp;

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

        if (grid[x][y] > 0.0) {
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

void get_new_generation(){

    
    alives = 0;

    int i, j;

    #pragma omp parallel for private(j) reduction(+:alives)
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            
            switch(get_neighbors(grid, i, j)){
                case 2:
                    if(grid[i][j] > 0){
                        
                        new_grid[i][j] = mean_from_neighbors(grid, i, j) > 0 ? 1.0 : 0; 
                        alives++;
                    } else  new_grid[i][j] = 0; 
                   
                    break;
                case 3:
                    new_grid[i][j] = mean_from_neighbors(grid, i, j) > 0 ? 1.0 : 0; 
                    alives++;
                    break;
                default:
                    new_grid[i][j] = 0;
                    break;
                
            }
        }
    }

    temp = grid;
    grid = new_grid;
    new_grid = temp;


}

int main(int argc, char *argv[]){

    if(argc < 2) {
        printf("Por favor, forneça um número inteiro como argumento.\n");
        return 1;
    }

    int num_threads = atoi(argv[1]);

    omp_set_num_threads(num_threads);
    grid = alloc_grid();
    new_grid  = alloc_grid();
    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    //GLIDER
    int lin = 1, col = 1;

    grid[lin ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;


    //R-pentomino
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
        get_new_generation();
    }
    printf("geração %d:  %d vivos\n", i, alives);
    
    gettimeofday(&end, NULL);
    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
    printf("Tempo gasto: %f milisegundos\n", mtime);

    desalloc_grid(grid);
    desalloc_grid(new_grid);
    
}
