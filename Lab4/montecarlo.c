#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>  
#include <mpi.h>   
#include <omp.h>    

#define NUM_CIRCLES 100 // broj krugova

// struktura za krug
typedef struct {
    double x, y, r; // centar (x,y) i radijus
} Circle;

// provjera da li je točka unutar kruga
int inside(double x, double y, Circle c) {
    double dx = x - c.x; // udaljenost po x
    double dy = y - c.y; // udaljenost po y
    return dx*dx + dy*dy <= c.r*c.r; // jednadžba kruga
}

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv); // inicijalizacija MPI

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // ID procesa
    MPI_Comm_size(MPI_COMM_WORLD, &size); // broj procesa

    long long N = atoll(argv[1]); // broj uzoraka
    long long local_n = N / size; // uzorci po procesu

    Circle circles[NUM_CIRCLES]; // niz krugova

    srand(42); // isti krugovi za sve procese
    for (int i = 0; i < NUM_CIRCLES; i++) {
        circles[i].x = rand()/(double)RAND_MAX; // x koordinata
        circles[i].y = rand()/(double)RAND_MAX; // y koordinata
        circles[i].r = 0.1 * rand()/(double)RAND_MAX; // radijus
    }

    long long hits = 0; // broj točaka unutar barem jednog kruga

    double start = MPI_Wtime(); // start mjerenja

    #pragma omp parallel reduction(+:hits) // paralelno uz sigurno zbrajanje
    {
        unsigned int seed = rank + omp_get_thread_num(); // seed po threadu

        #pragma omp for // podjela posla
        for (long long i = 0; i < local_n; i++) {

            // generiranje slučajne točke u kvadratu
            double x = rand_r(&seed)/(double)RAND_MAX;
            double y = rand_r(&seed)/(double)RAND_MAX;

            // provjera da li je unutar nekog kruga
            for (int j = 0; j < NUM_CIRCLES; j++) {
                if (inside(x, y, circles[j])) {
                    hits++; // povećaj ako je unutra
                    break;  // ne treba dalje provjeravati
                }
            }
        }
    }

    long long total; // ukupni broj pogodaka

    // zbrajanje rezultata svih procesa
    MPI_Reduce(&hits, &total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime(); // kraj mjerenja

    if (rank == 0) {
        printf("Fraction: %f\n", (double)total / N); // procjena površine
        printf("Time: %f\n", end - start);           // vrijeme izvršavanja
    }

    MPI_Finalize(); // završetak MPI
}