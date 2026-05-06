#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define BINS 256

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 2) {
        if (rank == 0)
            printf("Usage: %s N\n", argv[0]);
        MPI_Finalize();
        return 0;
    }

    long long N = atoll(argv[1]);

    long long local_n = N / size;

    int *data = malloc(local_n * sizeof(int));

    int local_hist[BINS] = {0};
    int global_hist[BINS] = {0};

    unsigned int seed = rank;

    for (long long i = 0; i < local_n; i++) {
        data[i] = rand_r(&seed) % BINS;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    #pragma omp parallel for
    for (long long i = 0; i < local_n; i++) {
        int val = data[i];

        #pragma omp atomic
        local_hist[val]++;
    }

    MPI_Reduce(local_hist, global_hist, BINS, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double end = MPI_Wtime();

    if (rank == 0) {
        printf("Histogram time: %f\n", end - start);
    }

    free(data);
    MPI_Finalize();

    return 0;
}