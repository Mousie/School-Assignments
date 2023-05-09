#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

unsigned long long int N[] = {100000000, 1000000000, 10000000000};
unsigned int tasks[] = {2,4,8};
unsigned long long int *sums;	// Global array for threads to store sums in. Could have made it not global and passed in a pointer as an arg.


void *sumSubset(void *args){
	long long int sum = 0;
	int id = *(long long int*)args;	// Get first arg, the thread ID
	long long int range = *(long long int*)(args+sizeof(long long int));	// Second arg, the range of the function.
	long long int lower = id*range;	// Determine the lower bound
	long long int upper = (id+1)*range;	// Determine the upper boudn
	for(long long int i = lower; i < upper; i++) sum += i;
	sums[id] = sum;	// Store thread result in global sum.
}


void sumOfN(unsigned long long int n, unsigned int numOfThreads){
	clock_t start, end;
	double timeUsed;
	sums = malloc(sizeof(long long int)*numOfThreads);	// Allocate memory for subsums.
	unsigned long long int sum = 0;
	unsigned long long int range = n/numOfThreads;	// Divvy up work
	pthread_t threads[numOfThreads];
	start = clock();
	long long int args[numOfThreads][2];	// Give threads their data chunks
	for(int i = 0; i < numOfThreads; i++){
		threads[i] = i;
		args[i][0] = i;
		args[i][1] = range;
		pthread_create(&threads[i], NULL, sumSubset, &args[i]);
	}
	
	for(int i = 0; i < numOfThreads; i++){
		pthread_join(threads[i], NULL);	// Wait for threads
		sum += sums[i];	// After they're done, add to the sum
	}	
	end = clock();
	printf("N: %llu Value: %llu Threads: %d Time Spent: %f\n", n, sum, numOfThreads, (double)(end-start)/CLOCKS_PER_SEC);
}

int main(int argc, char *argv[]){
	for(int i = 0; i < sizeof(N)/sizeof(N[0]); i++){
		for(int j = 0; j < sizeof(tasks)/sizeof(tasks[0]); j++){
			sumOfN(N[i], tasks[j]);
		}
	}
	return 0;
}
