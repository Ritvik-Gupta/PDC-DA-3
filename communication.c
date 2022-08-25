#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define BUF_SIZE 100
#define PROFILE_ITERATIONS 100
#define DATASET_SIZE 5

void generate_rand_data(double data []) {
	for (int i = 0; i < DATASET_SIZE; ++i)
		data[i] = (double)(rand() % 100) / 10.0;
}

void print_data(double data []) {
	for (int i = 0; i < DATASET_SIZE; ++i)
		printf("%g\t", data[i]);
	printf("\n\n");
}

typedef struct {
	double longest_time, avg_time;
} ProfileResult;

ProfileResult profile_for_send(int (*mpi_send_function)(const void*, int, MPI_Datatype, int, int, MPI_Comm), double dataset []) {
	double start, end, time_taken, total_time;
	ProfileResult res;

	for (int i = 0; i < PROFILE_ITERATIONS; ++i) {
		start = MPI_Wtime();
		mpi_send_function(&dataset, DATASET_SIZE, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
		end = MPI_Wtime();

		time_taken = end - start;
		total_time += time_taken;
		if (time_taken > res.longest_time)
			res.longest_time = time_taken;
	}
	res.avg_time = total_time / (double)PROFILE_ITERATIONS;

	return res;
}

void main(int argc, char* argv []) {
	srand(time(NULL));

	int rank, world_size;
	ProfileResult prf;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	if (world_size != 2) {
		printf("\n========\tInvalid Number of Processes Detected\t=========\n");
		exit(1);
	}

	double* buffer = malloc(sizeof(double) * BUF_SIZE);
	int buf_size = sizeof(double) * BUF_SIZE + MPI_BSEND_OVERHEAD;
	MPI_Buffer_attach(buffer, buf_size);

	double value[DATASET_SIZE];
	if (rank == 0) {
		generate_rand_data(value);
		prf = profile_for_send(MPI_Send, value);
		printf("Standard Sent value in\nAverage = %.10g s\tLongest = %.10g s\n", prf.avg_time, prf.longest_time);
		print_data(value);

		generate_rand_data(value);
		prf = profile_for_send(MPI_Ssend, value);
		printf("Synchronous Sent value in\nAverage = %.10g s\tLongest = %.10g s\n", prf.avg_time, prf.longest_time);
		print_data(value);

		generate_rand_data(value);
		prf = profile_for_send(MPI_Bsend, value);
		printf("Buffered Sent value in\nAverage = %.10g s\tLongest = %.10g s\n", prf.avg_time, prf.longest_time);
		print_data(value);

		generate_rand_data(value);
		prf = profile_for_send(MPI_Rsend, value);
		printf("Ready Sent value in\nAverage = %.10g s\tLongest = %.10g s\n", prf.avg_time, prf.longest_time);
		print_data(value);

	} else if (rank == 1) {
		for (int i = 0; i < 4 * PROFILE_ITERATIONS; ++i) {
			MPI_Recv(&value, DATASET_SIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	MPI_Buffer_detach(&buffer, &buf_size);
	MPI_Finalize();
}
