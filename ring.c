#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#define TIME_SIGNIFICANCE 100.0

double truncated_time() {
	double time = MPI_Wtime();
	time -= (double)(int)(time / TIME_SIGNIFICANCE) * TIME_SIGNIFICANCE;
	return time;
}

void main(int argc, char* argv[]) {
	srand(time(NULL));

	double curr_time;
	int rank, world_size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	int value;
	int next_rank = (rank + 1) % world_size, prev_rank = (rank + world_size - 1) % world_size;
	switch(rank) {
	case 0:
		value = rand() % 100;
		MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
		curr_time = truncated_time();
		printf("Sent data ( =%d ) from %d to %d at time %.20g\n", value, rank, next_rank, curr_time);

		MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		curr_time = truncated_time();
		printf("Received data ( =%d ) from %d to %d at time %.20g\n", value, prev_rank, rank, curr_time);
	break;

	default:
		MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		curr_time = truncated_time();
		printf("Received data ( =%d ) from %d to %d at time %.20g\n", value, prev_rank, rank, curr_time);

		MPI_Send(&value, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
		curr_time = truncated_time();
		printf("Send data ( =%d ) from %d to %d at time %.20g\n", value, rank, next_rank, curr_time);	
	break;
	} 

	MPI_Finalize();
}
