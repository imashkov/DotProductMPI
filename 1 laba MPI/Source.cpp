#include <iostream>
#include "mpi.h"

int main(int argc, char** argv)
{
	double start_time, finish_time, seq_time_result, par_time_result;
	const long n = 200000000;
	long seq_sum = 0, par_sum = 0, tmp = 0;
	int size, rank, i;
	bool ind = 0;
	MPI_Status status;
	long* a = new long[n];
	long* b = new long[n];
	for (long i = 0; i < n - 1; i++)
	{
		a[i] = 4999294 * (int)pow(-1, i % 2);
		b[i] = 3789640;
	}
	for (long i = n - 1; i < n; i++)
	{
		a[i] = 4999;
		b[i] = 37896;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank == 0)
	{
		std::cout << "Sequential processing:\n";
		start_time = MPI_Wtime();
		for (long i = 0; i < n; i++)
		{
			seq_sum += a[i] * b[i];
		}
		finish_time = MPI_Wtime();
		seq_time_result = 1000 * (finish_time - start_time);
		std::cout << "Dot product: " << seq_sum << std::endl;
		std::cout << "Time elapsed: " << seq_time_result << " ms\n";
		std::cout << "Parallel processing:\n";
	}




	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	if (rank == 0)
	{
		for (i = 0; i < n / 4; i++)
		{
			par_sum += a[i] * b[i];
		}
	}
	if (rank == 1)
	{
		for (i = n / 4; i < n / 2; i++)
		{
			par_sum += a[i] * b[i];
		}
	}
	if (rank == 2)
	{
		for (i = n / 2; i < 3 * n / 4; i++)
		{
			par_sum += a[i] * b[i];
		}
	}
	if (rank == 3)
	{
		for (i = 3 * n / 4; i < n; i++)
		{
			par_sum += a[i] * b[i];
		}
	}

	if (rank)
		MPI_Rsend(&par_sum, 1, MPI_LONG, 0, 66, MPI_COMM_WORLD);
	if (!rank)
	{
		MPI_Recv(&tmp, 1, MPI_LONG, MPI_ANY_SOURCE, 66, MPI_COMM_WORLD, &status);
		par_sum += tmp;
		tmp = 0;
		MPI_Recv(&tmp, 1, MPI_LONG, MPI_ANY_SOURCE, 66, MPI_COMM_WORLD, &status);
		par_sum += tmp;
		tmp = 0;
		MPI_Recv(&tmp, 1, MPI_LONG, MPI_ANY_SOURCE, 66, MPI_COMM_WORLD, &status);
		par_sum += tmp;
		tmp = 0;
		finish_time = MPI_Wtime();


		par_time_result = 1000 * (finish_time - start_time);
		std::cout << "Dot product: " << par_sum << std::endl;
		std::cout << "Time elapsed: " << par_time_result << " ms\n";
		if (seq_sum == par_sum)
			std::cout << "Sanity check: true\n";
		else
			std::cout << "Sanity check: false\n";
		std::cout << "Acceleration: " << round(seq_time_result / par_time_result * 100) / 100 << "x\n";
	}

	MPI_Finalize();
	delete[]a;
	delete[]b;
    return 0;
}

