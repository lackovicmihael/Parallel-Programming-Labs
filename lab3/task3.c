#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 512;
    int local_rows = N / size;
    double *A = NULL, *B = NULL, *C = NULL;
    if (rank == 0) {
        A = (double*)malloc(N * N * sizeof(double));
        B = (double*)malloc(N * N * sizeof(double));
        C = (double*)malloc(N * N * sizeof(double));
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                A[i*N + j] = 1.0;
                B[i*N + j] = 1.0;
            }
    }

    double *local_A = (double*)malloc(local_rows * N * sizeof(double));
    double *local_C = (double*)malloc(local_rows * N * sizeof(double));
    double *B_all = (double*)malloc(N * N * sizeof(double));

    double start = MPI_Wtime();

    MPI_Scatter(A, local_rows * N, MPI_DOUBLE,
                local_A, local_rows * N, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < N*N; i++) B_all[i] = B[i];
    }
    MPI_Bcast(B_all, N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_rows; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < N; k++) {
                sum += local_A[i*N + k] * B_all[k*N + j];
            }
            local_C[i*N + j] = sum;
        }
    }

    MPI_Gather(local_C, local_rows * N, MPI_DOUBLE,
               C, local_rows * N, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    double time = MPI_Wtime() - start;

    if (rank == 0) {
        printf("Vrijeme paralelnog množenja (N=%d, np=%d): %.4f sekundi\n", N, size, time);

        int correct = 1;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                double sum = 0.0;
                for (int k = 0; k < N; k++) {
                    sum += A[i*N + k] * B[k*N + j];
                }
                if (fabs(C[i*N + j] - sum) > 1e-5) correct = 0;
            }
        }
        printf("Validacija protiv serijskog baseline-a: %s\n", correct ? "CORRECT" : "FAILED");

        free(A); free(B); free(C);
    }

    free(local_A); free(local_C); free(B_all);
    MPI_Finalize();
    return 0;
}