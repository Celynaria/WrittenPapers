#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <CL/cl.h>
#include "simple.h"

#define DATA_SIZE 10240000

const char *KernelSource =                 "\n"
	"long euler (unsigned long n) {						  \n"	
   "	long length =0;							  \n"
   "   long i = 0;                                                          \n"
   "	if(n==1) {									  \n" 
   "		return 1;								  \n"	
	"}													  \n"
	"	length = n;									  \n"
   "	if(n%2 == 0) { 							  \n"
	"	length = length / 2;		  \n"
	"	while(n%2 == 0) {							  \n"	
	"		n = n / 2;								  \n"
	"	}												  \n"	
	"}													  \n"	
	"for (i = 3; i*i <= n; i+=2) {			  \n"
	"	if(n%i == 0) {								  \n"
	"		length = length - (length / i);    \n"
	"	}												  \n"
	"	while(n%i == 0) {							  \n"
	"		n = n / i;								  \n"
	"		}											  \n"
   "}													  \n"
   "if(n > 1) {									  \n"
	"	length = length - length / n;		  \n"
	"}													  \n"
	"return length;								  \n"	
   "}													  \n"	
   "__kernel void sumTotient(               \n"
   "   __global float* input,                \n"
   "   __global float* output,               \n"
   "   const unsigned int count)            \n"
   "{                                       \n"
   "   long tid = get_global_id(0);         \n"
   "     output[tid] += euler(input[tid]);  \n"
   "}                                       \n"
   "\n";
   
 struct timespec start, stop;

void printTimeElapsed( char *text)
{
  double elapsed = (stop.tv_sec -start.tv_sec)*1000.0
                  + (double)(stop.tv_nsec -start.tv_nsec)/1000000.0;
  printf( "%s: %f msec\n", text, elapsed);
}

/*void timeDirectImplementation( int count, float* data, float* results)
{
  clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start);
  for (int i = 0; i < count; i++)
    results[i] = data[i] * data[i];
  clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &stop);
  printTimeElapsed( "kernel equivalent on host");
}*/

int main (int argc, char * argv[])
{
  cl_int err;
  cl_kernel kernel;
  size_t global[1];
  size_t local[1];
  
  long lower, upper, num_procs;
  sscanf(argv[1], "%ld", &lower);
  sscanf(argv[2], "%ld", &upper);
  sscanf(argv[3], "%ld", &num_procs);
  
  if( argc <2) {
    local[0] = 32;
  } else {
    local[0] = atoi(argv[3]);
  }

  printf( "work group size: %d\n", (int)local[0]);

  

  /* Create data for the run.  */
  float *data = NULL;                /* Original data set given to device.  */
  float *results = NULL;             /* Results returned from device.  */
  long  sum_t;                       /* Number of correct results returned.  */
  

  
  //int count = DATA_SIZE;
  long count = (upper - lower) + 1;
  global[0] = count;
  

  data = (float *) malloc (count * sizeof (float));
  results = (float *) malloc (count * sizeof (float));

  /* Fill the vector with random float values.  */
  /*for (int i = 0; i < count; i++)
    data[i] = rand () / (float) RAND_MAX;*/
    for(int i = 0; i < count; i++) {
    	data[i] = lower + i;
    }

  clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start);
  err = initGPU();
 
  
  if( err == CL_SUCCESS) {
    kernel = setupKernel( KernelSource, "sumTotient", 3, FloatArr, count, data,
                                                     FloatArr, count, results,
                                                     IntConst, count);

    runKernel( kernel, 1, global, local);
  
   clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &stop);

   printKernelTime();
   printTimeElapsed( "GPU time spent");

    /* Validate our results.  */
    /*correct = 0;
    for (int i = 0; i < count; i++)
      if (results[i] == data[i] * data[i])
        correct++;*/
     sum_t = 0;
     for(long i = 0; i <= count; i++) {
     	sum_t = sum_t + results[i];
     }

    /* Print a brief summary detailing the results.  */
   /*printf ("Computed %d/%d %2.0f%% correct values\n", correct, count,
            (float)count/correct*100.f);*/
    printf("The sum is %ld\n", sum_t);

    err = clReleaseKernel (kernel);
    err = freeDevice();

   // timeDirectImplementation( count, data, results);
    
  }


  return 0;
}

