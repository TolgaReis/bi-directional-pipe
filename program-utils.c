#include "program-utils.h"
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>

void matrix_mult(const int row1, const int col1, const int row2, const int col2, 
                 const int first_matrix[], const int second_matrix[], int result_matrix[]) 
{
    int size = row1*col2;
    for (int i = 0; i < row1; i++) 
    {
        for (int j = 0; j < col2; j++) 
        {
            int element = 0;
            for (int k = 0; k < col1; k++)
            {
                element += first_matrix[i * col1 + k] * second_matrix[k * col2 + j];
            }
            result_matrix[i * col2 + j] = element;
        }
    }
}

void create_fd(int* fd_input, int* fd_input_2, const char* input_path, const char* input_path_2)
{
    *fd_input = open(input_path, O_RDONLY);
	if(*fd_input == -1)
	{
		perror("inputPathA: ");
        exit(EXIT_FAILURE);
	}

	*fd_input_2 = open(input_path_2, O_RDONLY);
	if(*fd_input_2 == -1)
	{
        perror("inputPathB: ");
		exit(EXIT_FAILURE);
	}
}

void handle_matrix(const int fd, int* matrix, const int n)
{
    int read_bytes = 0;
    int size = (int)pow(2.0, (double)n * 2);
    char char_matrix[size];
    if(!((read_bytes = read(fd, char_matrix, size)) == -1) && (errno == EINTR))
    {
		perror("Reading file: ");
        exit(EXIT_FAILURE);
    }
    if(read_bytes != size) 
    {
        fprintf(stderr, "There are not sufficient characters in the file!\n");
        exit(EXIT_FAILURE);
    }
    char_to_int(char_matrix, matrix, size);
    close(fd);
}

void char_to_int(const char* char_matrix, int* matrix, const int n)
{
    for (int i = 0; i < n; i++)
    {
        matrix[i] = (int)char_matrix[i];
    }
}