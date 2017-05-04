/*
 ============================================================================
 Name        : MPITotient.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int isdigit(int c);
long phi(long n);
//double sqrt(double x);

long phi(long n) {
  long length, i, t;

  length = 0;
  for (i = 1; i <= n; i++) {

    long x = n;
    long y = i;

    while(y != 0) {
     t = x % y;
     x = y;
     y = t;
    }

    if(x == 1) {
     length++;
    }

  }//END of for loop
  return length;
}
/*
 * Modify phi method
 * Uncomment for performance testing
 */
//long phi(long n) {
//	long length = 0;
//	long i = 0;
//	// skip number 1 (phi(1)== 1)
//	if (n == 1) {
//		return 1;
//	}
//	length = n;
//	//  next factoring out 2
//	if (n % 2 == 0) {
//		length = length / 2;
//		while (n % 2 == 0) {
//			n = n / 2;
//		}
//	}
//	//then factoring out 3 to sqrt(n)
//	for (i = 3; i <= (int) sqrt(n); i += 2) {
//		if (n % i == 0) {
//			length = length - (length / i);
//			while (n % i == 0) {
//				n = n / i;
//			}
//		}
//	}
//	//if n > 1, it is the last factor
//	if (n > 1) {
//		length = length - length / n;
//	}
//	return length;
//}

long sumTotient(long lower, long upper) {
	long sum = 0;
	long i;

	for (i = lower; i <= upper; i++)
		sum = sum + phi(i);
	return sum;
}

int main(int argc, char * argv[]) {
	long lower, upper, localSum;
	int p, id;
	long sum = 0;
	//double start, end;
	/* Red tape */
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	//MPI_Barrier(MPI_COMM_WORLD);
	//start = MPI_Wtime();
	/* Begin Parallel Application Block */

	/*The first process id will split the range of number into multiple chucks
	 *the size of chuck is depended on the size of process
	 */
	if (id == 0) {
		//Checking simple conditions for arguments before distributing the chucks
		if (argc != 3) {
			printf("not 2 arguments\n");
			return 1;
		}
		sscanf(argv[1], "%ld", &lower);
		sscanf(argv[2], "%ld", &upper);

		if (isdigit(lower) && isdigit(upper)) {
			printf("Arguments must be numbers\n");
			return 1;
		}
		if (lower > upper) {
			printf("The first argument must less than the second argument\n");
			return 1;
		}
		/*
		 * Begin splitting the range into chunks the file
		 */
		int fullRange = upper - lower + 1;
		int chunkRange = fullRange / p;
		//sending message to other process id
		long localUpper = upper;
		long localLower = lower;
		for (int i = 0; i < p; i++) {
			localUpper = localLower + chunkRange - 1;
			if (i == 0) {
				sum = sumTotient(localLower, localUpper);
				localLower = localUpper + 1;
			}else{
				if(i==p-1){
					localUpper = upper;
				}
//				printf("Lower is %ld\n", localLower);
//				printf("Upper is %ld\n", localUpper);
				MPI_Send(&localLower, 1, MPI_LONG, i, 1, MPI_COMM_WORLD);
				MPI_Send(&localUpper, 1, MPI_LONG, i, 1, MPI_COMM_WORLD);
				localLower = localUpper + 1;
			}
		}

		//Creating the waiting point for all process
		for (int i = 1; i < p; i++) {
			MPI_Recv(&localSum, 1, MPI_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD,
					&status);
			sum = sum + localSum;
		}
		printf("C: Sum of Totients  between [%ld..%ld] is %ld\n", lower, upper,
				sum);
		//end = MPI_Wtime();
		//printf("Total execution time is %f\n", end);
		/*Else other parallel process will receive the chunks
		 * and find the
		 */
	} else {
		//get lower and upper
		MPI_Recv(&lower, 1, MPI_LONG, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(&upper, 1, MPI_LONG, 0, 1, MPI_COMM_WORLD, &status);

		//Calling sumTotient()
		localSum = sumTotient(lower, upper);
//		printf("C: Sum of Totients  between [%ld..%ld] is %ld\n", lower, upper,
//				localSum);
		//Now send back to process 0
		MPI_Send(&localSum, 1, MPI_LONG, 0, 0, MPI_COMM_WORLD);
	}
	/* End  Parallel Application Block */
	/* Red tape */

	MPI_Finalize();
	return 0;
}
