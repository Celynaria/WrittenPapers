#include <stdio.h>
#include <mpi.h>
#include <time.h>
#define SENDING_TAG 9000
#define RETURNING_TAG 9001

long euler (long n);

long euler(long n) {
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
  
}//END of euler function

int main(int argc, char ** argv) { 
  
   MPI_Init(NULL, NULL);
   
	long lower, upper;
	
	MPI_Status status;
	long sum, partial_sum;
	int my_id, root_id, i, num_rows, num_procs, helper, begin, finish, proc_id, avg_rows_per_process;
	
	sum = 0;
	
	root_id = 0;
	
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

	if (argc != 3) {
		printf("not 2 arguments\n");
		return 1;
	}
  
	sscanf(argv[1], "%ld", &lower);
	sscanf(argv[2], "%ld", &upper);
	
		//If I am the root
	if(my_id == root_id) {
		//Condition to cater the situation where lower is not begin with 1
		num_rows = (upper - lower) + 1;
				
		avg_rows_per_process = num_rows / num_procs;
		
		//Distribute tasks to other processor
		for (proc_id = 1; proc_id < num_procs; ++proc_id) {
			//The last bit of the input array might not be of avg_rows_per_process length
			
			if(proc_id == (num_procs - 1)) {
				begin = proc_id * avg_rows_per_process + lower;
				finish = upper;
				
				//Send start and end index of the last bit to the last process
				MPI_Send(&begin, 1, MPI_INT, proc_id, SENDING_TAG, MPI_COMM_WORLD);
				MPI_Send(&finish, 1, MPI_INT, proc_id, SENDING_TAG, MPI_COMM_WORLD);
			}
			else {
				begin = lower + proc_id * avg_rows_per_process;
				finish = lower + (proc_id + 1) * avg_rows_per_process - 1;
				
				//Send start and end index of each process in their computation
				MPI_Send(&begin, 1, MPI_INT, proc_id, SENDING_TAG, MPI_COMM_WORLD);
				MPI_Send(&finish, 1, MPI_INT, proc_id, SENDING_TAG, MPI_COMM_WORLD);
			}
		}//End of for loop
		
		//Calculate the partial result (root)

		helper = lower;
		
		for(i = 0; i < avg_rows_per_process; i++) {
			sum = sum + euler(helper++);
		}
		
		//Sum up all process sum
		for(proc_id = 1; proc_id < num_procs; ++proc_id) {
				MPI_Recv(&partial_sum, 1, MPI_LONG, MPI_ANY_SOURCE, RETURNING_TAG, MPI_COMM_WORLD, &status);
			
				//Sender = status.MPI_Source;
				sum = sum + partial_sum;
		} 
		
	}//End of if
	//Worker process begin here
	else {
		//Identified the boundries
		MPI_Recv(&begin, 1, MPI_INT, root_id, SENDING_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&finish, 1, MPI_INT, root_id, SENDING_TAG, MPI_COMM_WORLD, &status);
				
		partial_sum = 0;
				
		//Calculate partial sum at slave process
		for(i = begin; i <= finish; ++i) {
			partial_sum = partial_sum + euler(i);
		}
		MPI_Send(&partial_sum, 1, MPI_LONG, root_id, RETURNING_TAG, MPI_COMM_WORLD);
	}//End of else 
			
	MPI_Finalize();
	printf("C: Sum of Totients  between [%ld..%ld] is %ld\n", lower, upper, sum);
		
	return 0;
}