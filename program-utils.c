#include "program-utils.h"
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

void matrix_mult(const int n, const int first_matrix[], const int second_matrix[n], int result_matrix[])
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = 0; j < n; j++) 
        {
            int element = 0;
            for (int k = 0; k < n; k++)
            {
                element += first_matrix[i * n + k] * second_matrix[k * n + j];
            }
            result_matrix[i * n + j] = element;
        }
    }
    
}

void create_fd(int* fd_input, int* fd_input_2, char* input_path, char* input_path_2)
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