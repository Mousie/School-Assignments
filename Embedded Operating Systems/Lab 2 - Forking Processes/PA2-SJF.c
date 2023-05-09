#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h>  
#include <stdlib.h>  
#include <sys/wait.h> 
#include <string.h> 
#include <time.h> 
#include <signal.h>
#include <sys/time.h>

/************************************************************************************************ 
		These DEFINE statements represent the workload size of each task and 
		the time quantum values for Round Robin scheduling for each task.
*************************************************************************************************/

#define WORKLOAD1 100000
#define WORKLOAD2 100000
#define WORKLOAD3 100000
#define WORKLOAD4 100000

/************************************************************************************************ 
					DO NOT CHANGE THE FUNCTION IMPLEMENTATION
*************************************************************************************************/
void myfunction(int param){

	int i = 2;
	int j, k;

	while(i < param){
		k = i; 
		for (j = 2; j <= k; j++)
		{
			if (k % j == 0){
				k = k/j;
				j--;
				if (k == 1){
					break;
				}
			}
		}
		i++;
	}
}
/************************************************************************************************/

int main(int argc, char const *argv[])
{
	pid_t pid1, pid2, pid3, pid4;
	int running1, running2, running3, running4;

	pid1 = fork();

	if (pid1 == 0){

		myfunction(WORKLOAD1);

		exit(0);
	}
	kill(pid1, SIGSTOP);

	pid2 = fork();

	if (pid2 == 0){

		myfunction(WORKLOAD2);

		exit(0);
	}
	kill(pid2, SIGSTOP);

	pid3 = fork();

	if (pid3 == 0){

		myfunction(WORKLOAD3);

		exit(0);
	}
	kill(pid3, SIGSTOP);

	pid4 = fork();

	if (pid4 == 0){

		myfunction(WORKLOAD4);

		exit(0);
	}
	kill(pid4, SIGSTOP);

	/************************************************************************************************ 
		At this point, all  newly-created child processes are stopped, and ready for scheduling.
	*************************************************************************************************/



	/************************************************************************************************
		- Scheduling code starts here
		- Below is a sample schedule. (which scheduling algorithm is this?)
		- For the assignment purposes, you have to replace this part with the other scheduling methods 
		to be implemented.
	************************************************************************************************/


	running1 = 1;
	running2 = 1;
	running3 = 1;
	running4 = 1;
	
	pid_t pids[4] = {pid1, pid2, pid3, pid4};
	int *running[4] = {&running1, &running2, &running3, &running4};
	struct timespec start, end[4];
	double totalTime;
	clock_gettime(CLOCK_REALTIME, &start);
// SJF (Shortest Job First)
	// Going to assume I can just order the jobs myself rather than an algorithm.

	for(int i = 3; i >= 0; i--){
		kill(pids[i], SIGCONT);
		while(*running[i]) waitpid(pids[i], running[i], WNOHANG);
		//waitpid(pids[i], running[i], 0);
		kill(pids[i], SIGSTOP);
		clock_gettime(CLOCK_REALTIME, &end[i]);
	}	
	int numOfValues = sizeof(end)/sizeof(end[0]);
	double time;
	for(int i = 0; i < numOfValues; i++){
		time = (end[i].tv_sec + end[i].tv_nsec*1e-9) - (start.tv_sec + start.tv_nsec * 1e-9);
		printf("ID: %d Time: %lf\n", i+1, time);
		totalTime += time;
	}
	clock_gettime(CLOCK_REALTIME, &end[0]);
	printf("\nAverage: %lf\n", (double)(totalTime)/4);
	
	/************************************************************************************************
		- Scheduling code ends here
	************************************************************************************************/

	return 0;
}