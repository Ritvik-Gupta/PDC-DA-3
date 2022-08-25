#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 20

void main(int argc, char* argv[]) {
	int rank, world_size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        if (world_size != 2) {
        	printf("\nInvalid Number of Processes Detected\n");
		exit(1);
	}

	double* buffer = malloc(BUF_SIZE * sizeof(double));
	int buffer_size = BUF_SIZE * sizeof(double) + MPI_BSEND_OVERHEAD;
	MPI_Buffer_attach(buffer, buffer_size);

	double mem;
	if (rank == 0) {
		for (mem = 0.0; mem < 20.0; mem += 2.0) {
			MPI_Bsend(&mem, 1, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			printf("Sent %g\n", mem);
			sleep(0.1);
		}
		printf("Finised Sending\n");
	} else if (rank == 1) {
		for (int i = 0; i < 10; ++i) {
			MPI_Recv(&mem, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Received %g\n", mem);
		}	
		printf("Finised Receiving\n");
	}

	MPI_Buffer_detach(&buffer, &buffer_size);
	MPI_Finalize();
}
