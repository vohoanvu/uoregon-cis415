#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <proc/readproc.h>
#include <errno.h>
#include "p1fxns.h"

#define NOTRUN 0
#define RUNING 1
#define PAUSED 2
#define EXITED 3

struct ProcessControlBlock
{
	char *	command;
	char **  args;
	pid_t	PID;
	int	status;
};

void LaunchProcess(struct ProcessControlBlock *launch);
void FreePCBs(struct ProcessControlBlock *Processes);
// this function counts how many lines are in the file
int line_count(char *filechar);
int line_count(char *filechar)
{
	FILE *fptr = fopen(filechar, "r");
	int count = 0;
	char chr;
	chr = getc(fptr);
	while ( chr != EOF ) {
		if ( chr == '\n') {
			count++;
		}
		chr = getc(fptr);
	}
	return count;
}

// this function return the number of words in a line
int get_word_count(char line[], char hold[]);
int get_word_count(char line[], char hold[])
{
	int index, word_count = 0;
	index = p1getword(line, 0, hold);
	while (index != -1) {
		word_count++;
		index = p1getword(line, index, hold); //update index
	}
	return word_count;
}

void PrintPCB(struct ProcessControlBlock *print);
void PrintPCB(struct ProcessControlBlock *print)
{
	printf("Enter: %s\n",__FUNCTION__);
	printf("\tCMD: %s\n", print->command);
	int i=0;
	while(print->args[i] != NULL)
	{
		i++;
	}
	printf("\tArgs: %d\n", i);
	i =0;
	while(print->args[i] != NULL)
	{
		printf("\t\tArg: %s\n", print->args[i]);
		i++;
	}
	printf("\tPID: %d\n",print->PID);
	switch(print->status)
	{
		case NOTRUN:
			printf("\tStatus: NOTRUN\n");
			break;
		case RUNING:
			printf("\tStatus: RUNING\n");
			break;
		case PAUSED:
			printf("\tStatus: PAUSED\n");
			break;
		case EXITED:
			printf("\tStatus: EXITED\n");
			break;
		default:
			printf("\tStatus: ERROR, invalid status: %d\n",print->status);
			break;
	}
	printf("Exit: %s\n",__FUNCTION__);
}



void sigusr1_handler(int signum)
{
	printf("Enter: %s\n",__FUNCTION__);
	Run =1;
	printf("Exit: %s\n",__FUNCTION__);
}

void sigusr2_handler(int signum)
{
	printf("Enter: %s\n",__FUNCTION__);
	// todo : Block other signals.
	sigprocmask(SIG_BLOCK, signal_set, NULL)	
	Exit =1;
	// todo : unblock other signals
	sigprocmask(SIG_UNBLOCK, signal_set, NULL)
	printf("Exit: %s\n",__FUNCTION__);
}


struct ProcessControlBlock *processes = NULL;
int numProg;

int main(int argc, char *argv[])
{
	FILE *in_f = fopen(argv[1], "r");

	if (argc < 2) {
		fprintf(stderr, "No input file!!!\n");
		return 1;
	} 

	if (in_f == NULL) {
		fprintf(stderr, "Error! Cannot open file!!\n");
		return 1;
	}

	char line[256];
	char NextWord[256];
	int i,j,len,numWord,sig;

	//signal attributes
	sig_flag = malloc(sizeof(int));
	*sig_flag = 1;
	signal(SIGALRM, alarm_handler);
	sigset_t sig_set;
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sig_set, NULL);
	PROCTAB* proc

	numProg = line_count(argv[1]);
	processes = (struct ProcessControlBlock *)malloc(sizeof(struct ProcessControlBlock) *numProg);
	// initialize
	for (i = 0; i < numProg; i++) {
		processes[i].PID = NOTRUN;
		processes[i].status = -1;
		//read in each line from input file, be careful with infinite loop tho!
		if (fgets(line, sizeof(line), in_f) != NULL) {
			printf("%s\n",line);
			//remove '/n' character
			len = strlen(line);
			if (line[len-1] == '\n') {
				line[len-1] = '\0';
			}
			numWord = get_word_count(line,NextWord);
			// allocating memory for each args in a line
			processes[i].args = (char **)malloc(sizeof(char*) * (numWord+1));
			// save each word into args
			p1getword(line,0,NextWord);
			//p1strcpy(processes[i].command, NextWord);
			processes[i].command = p1strdup(NextWord);
			int index = 0;
			for (j = 0; j < numWord; j++) {
				index = p1getword(line,index,NextWord);
				processes[i].args[j] = p1strdup(NextWord);
			}
			processes[i].args[j] = NULL;
		}
	}
	
	// launching all processes, going thru each processes an fork them
	for(i =0; i< numProg;i++)
	{	
		LaunchProcess(&processes[i]);
	}
	// This part is for part 2
	for (j = 0; j < numProg; j++) {
		printf("Process %d is waking up\n",processes.PID[j]);
		kill(processes.PID[j], SIGUSR1);
		//wait(NULL);
		//printf("child exited\n");
	}

	for (j = 0; j < numProg; j++) {
		printf("Stopping the Process %d\n",processes.PID[j]);
		kill(processes.PID[j], SIGSTOP);
		//wait(NULL);ssss
		//printf("child exited\n");
	}

	int p=0;
	// print all PCBs
	for(p=0; p< numProg; p++)
	{
		PrintPCB(&processes[p]);
	}

	//free all processes
	FreePCBs(processes);

	fclose(in_f);
	return 0;
}

void LaunchProcess(struct ProcessControlBlock *launch)
{
	printf("Enter: %s\n",__FUNCTION__);
	//int i;
	//todo:  Fork: Either run exec as child or save pid as parent.
	// launching all processes, going thru each processes an fork them
	//for (i = 0; i < numProgs; i++) {
	pid_t temp = fork();
	launch->PID = temp;

	if (temp < 0) {
		printf("Forking failed!");
		exit(1);
	}
	// child processes
	if (temp == 0) {
		// send start signal to all childrenn
		/*
		while (run == 0) {
			usleep(1);
		} */
		sigwait(&sigset, &sig);
		execvp(launch->command, launch->args);
		FreePCBs(launch);
		exit(1);
	}
	//}
	printf("Exit: %s\n",__FUNCTION__);	
}

void FreePCBs(struct ProcessControlBlock *Processes)
{
	printf("Enter: %s\n",__FUNCTION__);
	int i =0; 
	for( i =0; i< numProg; i++)
	{	
		free(Processes[i].command);
		// todo: Cleanup Processes[i]
		int itr =0;
		for (itr =0; Processes[i].args[itr] != NULL; itr++)
		{			
			free(Processes[i].args[itr]);
		}
	}
	free(Processes);
	Processes=NULL;
	printf("Exit: %s\n",__FUNCTION__);
}