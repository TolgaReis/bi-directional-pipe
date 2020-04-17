#ifndef PROGRAM_UTILS_H
#define PROGRAM_UTILS_H


/* calculates the product of two square matrices and returns the result matrix */
void matrix_mult(const int, const int*, const int*, int*);
/* gets file descriptors according to given paths */
void create_fd(int*, int*, char*, char*);

#endif