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

#define QUANTUM 3000
#define QUANTUM1 QUANTUM
#define QUANTUM2 QUANTUM
#define QUANTUM3 QUANTUM
#define QUANTUM4 QUANTUM
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
	
	unsigned int QUANTUMS[] = {QUANTUM1, QUANTUM2, QUANTUM3, QUANTUM4};
	pid_t pids[4] = {pid1, pid2, pid3, pid4};
	int *running[4] = {&running1, &running2, &running3, &running4};
	struct timespec start, end[4];
	char timeSet[4] = {0};
	double totalTime;
	clock_gettime(CLOCK_REALTIME, &start);
// Round Robin
	while (running1 > 0 || running2 > 0 || running3 > 0 || running4 > 0)
	{
		if (running1 > 0){
			kill(pid1, SIGCONT);
			usleep(QUANTUM1);
			kill(pid1, SIGSTOP);
		}
		waitpid(pid1, &running1, WNOHANG);
		if(running1 == 0 && timeSet[0] == 0){
			clock_gettime(CLOCK_REALTIME, &end[0]);
			timeSet[0] = 1;
		}
		if (running2 > 0){
			kill(pid2, SIGCONT);
			usleep(QUANTUM2);
			kill(pid2, SIGSTOP);
		}
		waitpid(pid2, &running2, WNOHANG);
		if(!running2 && timeSet[1] == 0){
			clock_gettime(CLOCK_REALTIME, &end[1]);
			timeSet[1] = 1;
		}
		if (running3 > 0){
			kill(pid3, SIGCONT);
			usleep(QUANTUM3);
			kill(pid3, SIGSTOP);
		}
		waitpid(pid3, &running3, WNOHANG);
		if(!running3 && timeSet[2] == 0){
			clock_gettime(CLOCK_REALTIME, &end[2]);
			timeSet[2] = 1;
		}
		if (running4 > 0){
			kill(pid4, SIGCONT);
			usleep(QUANTUM4);
			kill(pid4, SIGSTOP);
		}	
		waitpid(pid4, &running4, WNOHANG);
		if(!running4 && timeSet[3] == 0){
			clock_gettime(CLOCK_REALTIME, &end[3]);
			timeSet[3] = 1;
		}
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