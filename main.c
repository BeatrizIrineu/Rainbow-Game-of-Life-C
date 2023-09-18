#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>

#define N 2048
#define NUM_THREADS 8

sem_t mutex;
sem_t barrier;
int count = 0;
int generation = 0;


typedef struct {
    int start_row;
    int end_row;
} ThreadArgs;

pthread_t threads[NUM_THREADS];
ThreadArgs targs[NUM_THREADS];

float **new_grid;
float **grid;
    

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

void* process_matrix(void* args) {

    ThreadArgs* targs = (ThreadArgs*)args;
    float **temp;

    while(generation < 200){

        for (int i = targs->start_row; i < targs->end_row; i++) {
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
                        new_grid[i][j] = 0;
                        break;
                    
                }
            }
        }

        sem_wait(&mutex);
        count++;
        
        if (count == NUM_THREADS) {
            generation += 1;
            for (int i = 0; i < NUM_THREADS; i++) {
                sem_post(&barrier);
            }

            temp = grid;
            grid = new_grid;
            new_grid = temp;
            count = 0;
        }

        
        sem_post(&mutex);
        sem_wait(&barrier);

    }
    return NULL;
}


float** alloc_grid(){

    float **gri = calloc(sizeof(float*),N);
    for(int i = 0; i < N; i++){
        gri[i] = calloc(sizeof(float), N);
    }
    return gri;
}

void desalloc_grid(float** gri){
    for(int i = 0; i < N; i++){
        free(gri[i]);
    }

    free(gri);
}

void get_new_generation(){


    int rows_per_thread = N / NUM_THREADS;

     for (int i = 0; i < NUM_THREADS; i++) {
        targs[i].start_row = i * rows_per_thread;
        targs[i].end_row = (i + 1) * rows_per_thread;
        pthread_create(&threads[i], NULL, process_matrix, &targs[i]);
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

}



int main(){

   
    new_grid = alloc_grid();
    grid = alloc_grid();

    struct timeval start, end;
    long seconds, useconds;
    double mtime;

    

    int lin = 1, col = 1;

    grid[lin ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;

    sem_init(&mutex, 0, 1);
    sem_init(&barrier, 0, 0);

   
    gettimeofday(&start, NULL);
    
    get_new_generation(grid);
    
    gettimeofday(&end, NULL);

    seconds = end.tv_sec - start.tv_sec;
    useconds = end.tv_usec - start.tv_usec;

    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("Tempo gasto: %f milisegundos\n", mtime);

    desalloc_grid(grid);
    desalloc_grid(new_grid);

    sem_destroy(&mutex);
    sem_destroy(&barrier);
    
}
