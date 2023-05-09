#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

unsigned long long int N[] = {100000000, 1000000000, 10000000000};

int main(int argc, char *argv[]){
	unsigned long long int sum;
	clock_t start, end;
	double timeUsed;
	
	for(int i = 0; i < sizeof(N)/sizeof(N[0]); i++){
		start = clock();
		sum = 0;
		for(long long int j = 0; j < N[i]; j++)sum += j;
		end = clock();
		printf("Value: %llu Time Spent: %f\n", sum, (double)(end-start)/CLOCKS_PER_SEC);
	}
	return 0;
}
