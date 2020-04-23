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
#include "svd.h"

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

	side = (int)pow(2, n);
	size = (side * side);
	
	/* Initializes pipe mechanism for four childen processes */
	int pipe_fd_1[PROCESS_NUMBER][2];
	int pipe_fd_2[PROCESS_NUMBER][2];
	for(int i = 0; i < PROCESS_NUMBER; i++)
	{
		if(pipe(pipe_fd_1[i]) == -1)
		{
			perror("Pipe creation: ");
			exit(EXIT_FAILURE);
		}
		if(pipe(pipe_fd_2[i]) == -1)
		{
			perror("Pipe creation: ");
			exit(EXIT_FAILURE);
		}
	}

	for(int process_no = 0; process_no < 4; process_no++)
	{
		switch(child_pids[process_no] = fork())
		{
			case -1:
				perror("fork(): ");
				exit(EXIT_FAILURE);
			case 0:
				if(process_no < 4)
				{
					close(pipe_fd_2[process_no][0]);
					close(pipe_fd_1[process_no][1]);
					
					int matrix_a[size / 2];
					int matrix_b[size / 2];
					char temp[BLOCK_SIZE] = "";
					int read_bytes = 1;
					char character;
					int count = 0;
					while(read_bytes && count < size)
					{
						read_bytes = read(pipe_fd_1[process_no][0], &character, 1);
						if(character != ',')
						{
							strncat(temp, &character, 1);
						}
						else
						{
							if(count < size/2)
							{
								matrix_a[count] = atoi(temp);
							}
							else
							{
								matrix_b[count - size/2] = atoi(temp);
							}
							count++;
							strcpy(temp,"");
						}
					}
					matrix_mult(side / 2, side, side, side / 2, matrix_a, matrix_b, result);

					int matrix_res_size = BLOCK_SIZE;
					char* matrix_res = (char *)malloc(matrix_res_size);
					strcpy(temp, "");
					strcpy(matrix_res,"");
					for(int i = 0; i < ((int)pow(side, 2))/4; i++)
					{
						sprintf(temp, "%d,", result[i]);
						if(strlen(matrix_res) + strlen(temp) >= matrix_res_size)
						{
							matrix_res_size *= 2;
							matrix_res = (char *)realloc(matrix_res, matrix_res_size);
						}
						strcat(matrix_res, temp);
					}
					send_to_parent(pipe_fd_2[process_no][1], matrix_res);

					free(matrix_res);
					exit(EXIT_SUCCESS);
				}

			default:
				close(pipe_fd_2[process_no][1]);
				close(pipe_fd_1[process_no][0]);
				break;
		}
	}
				

	int bytesread = 0;
	int matrix_a[size];
	int matrix_b[size];
	int** matrix_res;
	matrix_res = (int **)calloc(side, sizeof(int *));
	for(int i = 0; i < side; i++)
	{ 
		matrix_res[i] = (int *)calloc(side, sizeof(int));
	}
	char* matrix_a_1 = (char*)malloc(BLOCK_SIZE);
	char* matrix_a_2 = (char*)malloc(BLOCK_SIZE);
	char* matrix_b_1 = (char*)malloc(BLOCK_SIZE);
	char* matrix_b_2 = (char*)malloc(BLOCK_SIZE);
	
	create_fd(&fd_input, &fd_input_2, input_file_path, input_file_path_2);
	handle_matrix(fd_input, matrix_a, size);
	handle_matrix(fd_input_2, matrix_b, size);
	create_quarters(matrix_a_1, matrix_a_2, matrix_b_1, matrix_b_2, matrix_a, matrix_b, size);
	
	send_to_child(pipe_fd_1[0][1], matrix_a_1, matrix_b_1);
	read_pipe(pipe_fd_2[0][0], side, 1, matrix_res);
	
	send_to_child(pipe_fd_1[1][1], matrix_a_1, matrix_b_2);
	read_pipe(pipe_fd_2[1][0], side, 2, matrix_res);

	send_to_child(pipe_fd_1[2][1], matrix_a_2, matrix_b_1);
	read_pipe(pipe_fd_2[2][0], side, 3, matrix_res);

	send_to_child(pipe_fd_1[3][1], matrix_a_2, matrix_b_2);
	read_pipe(pipe_fd_2[3][0], side, 4, matrix_res);

	free(matrix_a_1);
	free(matrix_a_2);
	free(matrix_b_1);
	free(matrix_b_2);

	float* singular_values = (float *)malloc(side * sizeof(float *));
	dsvd(matrix_res, side, singular_values);
	
	printf("\n===== Singular Values =====\n\n");
	for(int i = 0; i < side; i++)
		printf("%d. singular value: %.3lf \n", i, singular_values[i]);
	printf("\n");

	free(singular_values);

	for(int i = 0; i < side; i++)
    	free(matrix_res[i]);
	free(matrix_res);

	sigemptyset(&emptyMask);
	while (total_child > 0)
	{
		if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
		{
			perror("Sigsuspend: ");
		}
	}
	return 0;
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