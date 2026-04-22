#include <stdio.h>    
#include <stdlib.h>    
#include <mpi.h>    
#include <omp.h>  

int main(int argc, char** argv) {

    MPI_Init(&argc, &argv); 

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long long N = atoll(argv[1]);
    long long local_n = N / size; 

    int *data = malloc(local_n * sizeof(int));
    int *prefix = malloc(local_n * sizeof(int)); 

    unsigned int seed = rank; 
    for (long long i = 0; i < local_n; i++)
        data[i] = rand_r(&seed) % 10;

    double start = MPI_Wtime(); 

    prefix[0] = data[0];
    for (long long i = 1; i < local_n; i++)
        prefix[i] = prefix[i-1] + data[i]; 
    int local_sum = prefix[local_n - 1]; 
    int offset = 0;                     

    MPI_Exscan(&local_sum, &offset, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    #pragma omp parallel for 
    for (long long i = 0; i < local_n; i++)
        prefix[i] += offset; 

    double end = MPI_Wtime(); 
    if (rank == 0)
        printf("Scan time: %f\n", end - start);

    free(data);     
    free(prefix);
    MPI_Finalize(); 
}