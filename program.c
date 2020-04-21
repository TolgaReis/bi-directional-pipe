#include "program.h"
#include "program-utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

/* Macro Constants */
#define PATH_LENGTH 128
#define PROCESS_NUMBER 4

/* Function Declarations */
void handler(int);

/* Global Variables*/
static volatile int total_child = 0;
pid_t child_pids[4];

int main(int argc, char* argv[])
{
    /* FILE DESCRIPTORS */
    /* file descriptor for file that is path name of given by -i option */
    int fd_input;
    /* file descriptor for file that is path name of given by -j option */
    int fd_input_2;
	/* given -n parameter when program starts*/
	int n;
	/* one side lenth of the matrix */
	int side;
	/* total element number of the matrix */
	int size;
	/* given -i inputPathA parameter */
    char input_file_path[PATH_LENGTH];
	/* given -j inputPathB prameter */
    char input_file_path_2[PATH_LENGTH];
	/* initializes live child process number */
	total_child = PROCESS_NUMBER;
	    
	int option;

    /* if given argument numbers is not same as the format */
	if(argc != 7)
	{
		fprintf(stderr, "Wrong input option usage! Use: ./program -i inputPathA -j inputPathB -n 8\n");
		exit(EXIT_FAILURE);
	}

	while ((option = getopt (argc, argv, "ijn")) != -1)
  	{
	    switch (option)
	      {
	      case 'i':
	        strcpy(input_file_path, argv[optind]);
	        break;
	      case 'j':
	        strcpy(input_file_path_2, argv[optind]);
	        break;
		  case 'n':
			if((n = atoi(argv[optind])) <= 1)
			{
				fprintf(stderr, "Please enter a positive integer value for -n option!\n");
				exit(EXIT_FAILURE);
			}
			break;
	      default:
		  	fprintf(stderr, "Wrong input option usage! Use: ./program -i inputPathA -j inputPathB -n 8\n");
			exit(1);
	      }
	}

	sigset_t blockMask, emptyMask;
	struct sigaction sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
		perror("sigaction");

	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGCHLD);
	if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
		perror("sigprocmask");
	
	

	create_fd(&fd_input, &fd_input_2, input_file_path, input_file_path_2);
}

void handler(int signal)
{
    if(signal == SIGCHLD)
    {
    	int status; 
		int last_errno = errno;
		pid_t child_pid;
		while ((child_pid = waitpid(-1, &status, WNOHANG)) > 0)
		{
			printf("PID = %d was terminated!\n",child_pid);
			total_child--;
		}
		if (child_pid == -1 && errno != ECHILD)
		{
			perror("waitpid");
		}
		errno = last_errno;
    }
	else if(signal == SIGINT)
    {
    	for(int i = 0; i < PROCESS_NUMBER; i++)
    		kill(child_pids[i], SIGKILL);
    	exit(EXIT_SUCCESS);
    }
}