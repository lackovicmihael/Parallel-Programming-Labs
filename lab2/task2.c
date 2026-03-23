#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {

    int N = 10000000;

    int* data = malloc(N * sizeof(int));
    int hist[256];

    for (int i = 0; i < N; i++) {
        data[i] = rand() % 256;
    }

    // =========================
    // (a) SERIJSKA VERZIJA
    // =========================
    for (int i = 0; i < 256; i++) hist[i] = 0;

    double s1 = omp_get_wtime();

    for (int i = 0; i < N; i++) {
        hist[data[i]]++;
    }

    double s2 = omp_get_wtime();

    printf("Serial time: %f\n", s2 - s1);
    printf("Serial hist[0] = %d\n\n", hist[0]);

    // =========================
    // (b) PARALLEL + ATOMIC
    // =========================
    for (int i = 0; i < 256; i++) hist[i] = 0;

    double p1 = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        #pragma omp atomic
        hist[data[i]]++;
    }

    double p2 = omp_get_wtime();

    printf("Parallel (atomic) time: %f\n", p2 - p1);
    printf("Atomic hist[0] = %d\n\n", hist[0]);

    // =========================
    // (c) PARALLEL + CRITICAL
    // =========================
    for (int i = 0; i < 256; i++) hist[i] = 0;

    double c1 = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        #pragma omp critical
        {
            hist[data[i]]++;
        }
    }

    double c2 = omp_get_wtime();

    printf("Parallel (critical) time: %f\n", c2 - c1);
    printf("Critical hist[0] = %d\n\n", hist[0]);

    free(data);
    return 0;
}