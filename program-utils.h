#ifndef PROGRAM_UTILS_H
#define PROGRAM_UTILS_H


/* calculates the product of two matrices */
void matrix_mult(const int, const int, const int, const int, const int*, const int*, int*);
/* gets file descriptors according to given paths */
void create_fd(int*, int*, const char*, const char*);
/* reads the file with given file descriptor and converts it to square matrix */
void handle_matrix(const int, int*, const int);
/* converts char array to int array */
void char_to_int(const char*, int*, const int);

#endif