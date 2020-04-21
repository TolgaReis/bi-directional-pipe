#include "program-utils.h"
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>

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
    int size = (int)pow(2.0, n * 2);
    char char_matrix[size];
    if(!((read_bytes = read(fd, char_matrix, sizeof(char_matrix))) == -1) && (errno == EINTR))
    {
		perror("Reading file: ");
        exit(EXIT_FAILURE);
    }
    if(read_bytes != size) 
    {
        fprintf(stderr, "There are not sufficient characters in the file!\n");
        exit(EXIT_FAILURE);
    }
    char_matrix[size] = '\0';
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

void create_quarters(char* matrix_a_1, char* matrix_a_2, char* matrix_b_1, char* matrix_b_2,
                     const int* matrix_a, const int* matrix_b, const int n)
{
    int size = BLOCK_SIZE;
    char buf[32];
    for(int i = 0; i < n/2; i++)
    {
        sprintf(buf, "%d,", matrix_a[i]);
        if(strlen(matrix_a_1) + strlen(buf) >= size)
        {
            size *= 2;
            matrix_a_1 = (char *) realloc(matrix_a_1, size);
        }
        strcat(matrix_a_1, buf);
    }
    
    size = BLOCK_SIZE;
    for(int i = n / 2; i < n; i++)
    {
        sprintf(buf, "%d,", matrix_a[i]);
        if(strlen(matrix_a_2) + strlen(buf) >= size)
        {
            size *= 2;
            matrix_a_2 = (char *) realloc(matrix_a_2, size);
        }
        strcat(matrix_a_2, buf);
    }

    size = BLOCK_SIZE;
    for(int i = 0; i < n; i++)
    {
        sprintf(buf, "%d,", matrix_b[i]);
        if(strlen(matrix_b_1) + strlen(buf) >= size)
        {
            size *= 2;
            matrix_b_1 = (char *) realloc(matrix_b_1, size);
        }
        strcat(matrix_b_1, buf);
        if((i+1) % ((int)sqrt(n) / 2) == 0)
                i += ((int)sqrt(n) / 2);
    }

    size = BLOCK_SIZE;
    for(int i = ((int)sqrt(n) / 2); i < n; i++)
    {
        sprintf(buf, "%d,", matrix_b[i]);
        if(strlen(matrix_b_2) + strlen(buf) >= size)
        {
            size *= 2;
            matrix_b_2 = (char *) realloc(matrix_b_2, size);
        }
        strcat(matrix_b_2, buf);
        if((i+1) % ((int)sqrt(n) / 2) == 0)
                i += ((int)sqrt(n) / 2);
    }
}

void send_to_child(int fd, char* matrix_a, char* matrix_b)
{
    int bytes = strlen(matrix_a);
    int written_bytes = 0;
    char* bp = matrix_a;
    while(bytes > 0)
    {
        while(((written_bytes = write(fd, bp, bytes)) == -1 ) && (errno == EINTR));
        if (written_bytes < 0)
            break;
        bytes -= written_bytes;
        bp += written_bytes;
    }

    bytes = strlen(matrix_b);
    written_bytes = 0;
    bp = matrix_b;

    while(bytes > 0)
    {
        while(((written_bytes = write(fd, bp, bytes)) == -1 ) && (errno == EINTR));
        if (written_bytes < 0)
            break;
        bytes -= written_bytes;
        bp += written_bytes;
    }
}

void send_to_parent(int fd, char* matrix)
{
	int bytes = strlen(matrix);
	int written_bytes = 0;
	char* bp = matrix;
	while(bytes > 0)
	{
		while(((written_bytes = write(fd, bp, bytes)) == -1 ) && (errno == EINTR));
		if (written_bytes < 0)
			break;
		bytes -= written_bytes;
		bp += written_bytes;
	}

}

void read_pipe(int fd, int number2, int quart, int** matrix)
{
	char buffer[BLOCK_SIZE] = "";
	int size = number2*number2/4;
	int A[size];
	int bytesRead = 1;
	int count = 0;
	char ch;
	while(bytesRead && count < size)
	{
		bytesRead = read(fd, &ch, 1);
		if(ch != ',')
			strncat(buffer, &ch, 1);
		else
		{
			A[count] = atoi(buffer);
			strcpy(buffer,"");
			count++;
		}
	}

	int i, j, x = 0;

	if(quart == 2)
	{
		for(i = 0; i < number2/2; i++)
		{
			for(j = 0; j < number2/2; j++)
			{
				matrix[i][j] = A[x];
				x++;
			}
		}
	}
	else if(quart == 3)
	{
		for(i = 0; i < number2/2; i++)
		{
			for(j = number2/2; j < number2; j++)
			{
				matrix[i][j] = A[x];
				x++;
			}
		}
	}
	else if(quart == 4)
	{
		for(i = number2/2; i < number2; i++)
		{
			for(j = 0; j < number2/2; j++)
			{
				matrix[i][j] = A[x];
				x++;
			}
		}
	}
	else if(quart == 5)
	{
		for(i = number2/2; i < number2; i++)
		{
			for(j = number2/2; j < number2; j++)
			{
				matrix[i][j] = A[x];
				x++;
			}
		}
	}
	else
	{
		perror("wrong quart in reader");
		exit(1);
	}
}