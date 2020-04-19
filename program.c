#include "program.h"
#include "program-utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    /* FILE DESCRIPTORS */
    /* file descriptor for file that is path name of given by -i option */
    int fd_input;
    /* file descriptor for file that is path name of given by -j option */
    int fd_input_2;

    char input_file_path[PATH_LENGTH];
    char input_file_path_2[PATH_LENGTH];
	int n;

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
			if((n = atoi(argv[optind])) <= 0)
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
	create_fd(&fd_input, &fd_input_2, input_file_path, input_file_path_2);
    
}