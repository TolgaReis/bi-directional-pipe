#ifndef PROGRAM_UTILS_H
#define PROGRAM_UTILS_H

/* Macro Constants*/
#define BLOCK_SIZE 1024

/* calculates the product of two matrices */
void matrix_mult(const int, const int, const int, const int, const int*, const int*, int*);
/* gets file descriptors according to given paths */
void create_fd(int*, int*, const char*, const char*);
/* reads the file with given file descriptor and converts it to square matrix */
void handle_matrix(const int, int*, const int);
/* converts char array to int array */
void char_to_int(const char*, int*, const int);
/* creates quarters from A and B matrices */
void create_quarters(char*, char*, char*, char*, const int*, const int*, const int);
/* sends two quarters to child process to calculates matrix multiplication */
void send_to_child(int, char*, char*);
/* sends multiplicaiton result from child to parent process*/
void sent_to_parent(int, char*);
/* reads pipe when sending data from child to parent or parent to child process*/
void read_pipe(int, int, int, int**);

#endif