#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>  
#include <mpi.h>   
#include <omp.h>    

#define NUM_CIRCLES 100 

typedef struct {
    double x, y, r; 
} Circle;

int inside(double x, double y, Circle c) {
    double dx = x - c.x; 
    double dy = y - c.y; 
    return dx*dx + dy*dy <= c.r*c.r; 
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = atoll(argv[1]); 
    long long local_n = N / size; 

    Circle circles[NUM_CIRCLES];

    srand(42); 
    for (int i = 0; i < NUM_CIRCLES; i++) {
        circles[i].x = rand()/(double)RAND_MAX; 
        circles[i].y = rand()/(double)RAND_MAX; 
        circles[i].r = 0.1 * rand()/(double)RAND_MAX; 
    }

    long long hits = 0; 

    double start = MPI_Wtime(); 

    #pragma omp parallel reduction(+:hits) 
    {
        unsigned int seed = rank + omp_get_thread_num(); 

        #pragma omp for 
        for (long long i = 0; i < local_n; i++) {

            double x = rand_r(&seed)/(double)RAND_MAX;
            double y = rand_r(&seed)/(double)RAND_MAX;

            for (int j = 0; j < NUM_CIRCLES; j++) {
                if (inside(x, y, circles[j])) {
                    hits++;
                    break; 
                }
            }
        }
    }

    long long total; 

    MPI_Reduce(&hits, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime(); 

    if (rank == 0) {
        printf("Fraction: %f\n", (double)total / N); 
        printf("Time: %f\n", end - start);           
    }

    MPI_Finalize(); 
}