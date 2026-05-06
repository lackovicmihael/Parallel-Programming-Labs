#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define NUM_POINTS 100000000 // Total number of points

int main(int argc, char** argv) {
    int rank, size, local_count = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int points_per_process = NUM_POINTS / size;

    unsigned int seed = time(NULL) + rank;

    for (int i = 0; i < points_per_process; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x * x + y * y <= 1.0) {
            local_count++;
        }
    }

    int global_count;

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        double pi = 4.0 * global_count / NUM_POINTS;
        printf("Estimated π: %f\n", pi);
    }

    MPI_Finalize();
    return 0;
}