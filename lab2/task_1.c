#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {

    int N = 2000; // 500, 1000, 2000

    double* matrix = (double*)malloc(N * N * sizeof(double));

    double start = omp_get_wtime();
    
    // #pragma omp parallel for schedule(static) // static
    // #pragma omp parallel for schedule(dynamic, 100) // dynamic
    #pragma omp parallel for schedule(guided) // guided

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            matrix[i * N + j] = i + j;
        }
    }

    double end = omp_get_wtime();

    printf("Vrijeme: %f\n", end - start);

    printf("matrix[0] = %f\n", matrix[0]);
    printf("matrix[last] = %f\n", matrix[N*N - 1]);

    free(matrix);
    return 0;
}