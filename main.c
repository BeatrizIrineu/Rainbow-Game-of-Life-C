#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>

#define N 2048
#define NUM_THREADS 4

sem_t mutex;
sem_t barrier;
int count = 0;
int generation = 0;


typedef struct {
    int start_row;
    int end_row;
    float** grid;
    float **new_grid;
} ThreadArgs;

pthread_t threads[NUM_THREADS];
ThreadArgs targs[NUM_THREADS];

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

    while(generation < 50){

        for (int i = targs->start_row; i < targs->end_row; i++) {
            for (int j = 0; j < N; j++) {
                
                switch(get_neighbors(targs->grid, i, j)){
                    case 2:
                        if(targs->grid[i][j] == 1)
                            targs->new_grid[i][j] = 1;
                        break;
                    case 3:
                        targs->new_grid[i][j] = 1;
                        break;
                    default:
                         targs->new_grid[i][j] = 0;
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

            targs->grid = targs->new_grid;
            count = 0;
        }

        
        sem_post(&mutex);
        sem_wait(&barrier);

    }
    return NULL;
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

    int rows_per_thread = N / NUM_THREADS;

     for (int i = 0; i < NUM_THREADS; i++) {
        targs[i].start_row = i * rows_per_thread;
        targs[i].end_row = (i + 1) * rows_per_thread;
        targs[i].grid = grid;
        targs[i].new_grid = new_grid;
        pthread_create(&threads[i], NULL, process_matrix, &targs[i]);
    }


    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    
    desalloc_grid(grid);
    return new_grid;

}

int main(){
    float **grid = alloc_grid();
    

    int lin = 1, col = 1;

    grid[lin ][col+1] = 1.0;
    grid[lin+1][col+2] = 1.0;
    grid[lin+2][col  ] = 1.0;
    grid[lin+2][col+1] = 1.0;
    grid[lin+2][col+2] = 1.0;

    sem_init(&mutex, 0, 1);
    sem_init(&barrier, 0, 0);

   
    clock_t start_time = clock();
    
    
    grid = get_new_generation(grid);
    
    
    clock_t end_time = clock();

    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Tempo gasto: %f segundos\n", time_spent);

    desalloc_grid(grid);

    sem_destroy(&mutex);
    sem_destroy(&barrier);
    
}
