#include <stdio.h> 
#include <stdlib.h> 
#include <omp.h> 
#define NUM_POINTS 100000000 
 
int main() { 
    int count = 0, local_count; 
    omp_set_num_threads(4); 
 
    #pragma omp parallel reduction(+:local_count) 
    { 
        unsigned int seed = 42 + omp_get_thread_num(); // Thread-local seed 
        local_count = 0; 
 
        #pragma omp for 
        for (int i = 0; i < NUM_POINTS; i++) { 
            double x = (double)rand_r(&seed) / RAND_MAX; 
            double y = (double)rand_r(&seed) / RAND_MAX; 
            if (x*x + y*y <= 1) local_count++; 
        } 
    } 
 
    double pi = 4.0 * local_count / NUM_POINTS; 
    printf("Estimated π: %f\n", pi); 
    return 0; 
}