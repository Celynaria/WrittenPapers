// Totient.c - Sequential Euler Totient Function (C Version)
// compile: gcc -Wall -O -o TotientRange TotientRange.c
// run:     ./TotientRange lower_num uppper_num

// Greg Michaelson 14/10/2003
// Patrick Maier   29/01/2010 [enforced ANSI C compliance]

// This program calculates the sum of the totients between a lower and an
// upper limit using C longs. It is based on earlier work by:
// Phil Trinder, Nathan Charles, Hans-Wolfgang Loidl and Colin Runciman

#include <stdio.h>
#include <ctype.h>
#include <math.h>

// euler n = length (filter (relprime n) [1 .. n-1])

long euler(long n) {
	long length, i;
	int prime = 1;
	// skip number 1 (phi(1)== 1)
	if (n == 1) {
		return 1;
	}
	length = n;
	//using explicit formula φ(n) = nΠ(1 - 1/pj) pj is prime factor of n.
	//start with 2
	if (n % 2 == 0) {
		length = length/2;
		prime = 0;
	}
	//then 3 to squrt(n)
	for (i = 3; i <= (int) sqrt(n); i += 2) {
		if (n % i == 0) {
			length *= n * (1 - 1 / i);
			prime = 0;
		}
	}
	if(n%i == 0){
		length *= n * (1 - 1 / i);
	}
	if(prime){
		length-=1;
	}
	return length;
}

// sumTotient lower upper = sum (map euler [lower, lower+1 .. upper])

long sumTotient(long lower, long upper) {
	long sum, i;

	sum = 0;
	for (i = lower; i <= upper; i++)
		sum = sum + euler(i);
	return sum;
}

int main(int argc, char * argv[]) {
	long lower, upper;

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

	printf("C: Sum of Totients  between [%ld..%ld] is %ld\n", lower, upper,
			sumTotient(lower, upper));
	return 0;
}
