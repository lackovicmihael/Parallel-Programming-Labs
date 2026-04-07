#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int TOTAL = 10000000;
    int chunk = TOTAL / size;
    int *local_array = (int*)malloc(chunk * sizeof(int));
    int local_hist[256] = {0};
    int *global_hist = NULL;

    double start = MPI_Wtime();

    if (rank == 0) {
        global_hist = (int*)calloc(256, sizeof(int));
        int *full = (int*)malloc(TOTAL * sizeof(int));
        for (int i = 0; i < TOTAL; i++) full[i] = i % 256;

        double serial_start = MPI_Wtime();
        int serial_hist[256] = {0};
        for (int i = 0; i < TOTAL; i++) serial_hist[full[i]]++;
        double serial_time = MPI_Wtime() - serial_start;
        printf("Serijsko vrijeme: %.4f sekundi\n", serial_time);

        MPI_Request *reqs = (MPI_Request*)malloc((size-1) * sizeof(MPI_Request));
        for (int p = 1; p < size; p++) {
            MPI_Isend(&full[p*chunk], chunk, MPI_INT, p, 0, MPI_COMM_WORLD, &reqs[p-1]);
        }
        for (int i = 0; i < chunk; i++) local_array[i] = full[i];

        MPI_Waitall(size-1, reqs, MPI_STATUSES_IGNORE);
        free(reqs);
        free(full);
    } 
    else {
        MPI_Request req;
        MPI_Irecv(local_array, chunk, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);
        MPI_Wait(&req, MPI_STATUS_IGNORE);
    }

    for (int i = 0; i < chunk; i++) {
        local_hist[local_array[i]]++;
    }

    MPI_Reduce(local_hist, global_hist, 256, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double total_time = MPI_Wtime() - start;

    if (rank == 0) {
        printf("Paralelno vrijeme (distribucija + hist + reduce): %.4f sekundi\n", total_time);
        long long sum = 0;
        for (int i = 0; i < 256; i++) sum += global_hist[i];
        printf("Ukupno elemenata u histogramu: %lld (treba biti %d)\n", sum, TOTAL);
        free(global_hist);
    }

    free(local_array);
    MPI_Finalize();
    return 0;
}