#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) printf("Pokreni sa: mpirun -np 2 ./pingpong\n");
        MPI_Finalize();
        return 0;
    }

    float data = 42.0f;
    MPI_Status status;
    int tag = 17;

    if (rank == 0) {
        printf("Proces 0: šaljem original = %.2f procesu 1\n", data);
        MPI_Send(&data, 1, MPI_FLOAT, 1, tag, MPI_COMM_WORLD);

        MPI_Recv(&data, 1, MPI_FLOAT, 1, tag, MPI_COMM_WORLD, &status);
        printf("Proces 0: primio modificirani podatak = %.2f\n", data);
        printf("   Detalji poruke -> source: %d, tag: %d\n", status.MPI_SOURCE, status.MPI_TAG);
        int count;
        MPI_Get_count(&status, MPI_FLOAT, &count);
        printf("   Veličina poruke: %d elemenata\n", count);
    } 
    else if (rank == 1) {
        MPI_Recv(&data, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD, &status);
        printf("Proces 1: primio = %.2f\n", data);
        data = data * 2.0f;
        printf("Proces 1: modificirano na %.2f, šaljem natrag\n", data);
        MPI_Send(&data, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}