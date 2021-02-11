/**
 * @file utility.h
 * @author TendTo (https://github.com/TendTo)
 * 
 * @brief Utility functions
 * Collection of utility functions, usually used for debugging purposes
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRECISION 0.0001 //Error accepted when dealing with doubles

/**
 * Casts each value in the array from double to float
 * 
 * @param A Array to cast
 * @param n Length of the array
 * 
 * @return Pointer to the array with the correct type
 */
float *d_to_f_array(double *a, size_t n);

/**
 * Casts each value in the array from float to double
 * 
 * @param A Array to cast
 * @param n Length of the array
 * 
 * @return Pointer to the array with the correct type
 */
double *f_to_d_array(float *a, size_t n);

/**
 * Copies the given linear matrix A to a newly created linear matrix.
 * If a b vector is given, it is assimilated al a new column of the new matrix, wich will be (n X n + 1)
 * 
 * @param A Linear matrix to copy. If b is NULL, it will be assumed to have size (n X n + 1)
 * @param b Vector of the solutions to append as a new column. If NULL, the first matrix will be considered as having it already in it
 * @param n Number of unknowns
 * 
 * @return Complete matrix of size (n X n + 1)
 */
double *create_complete_matrix_lin(double *A, double *b, size_t n);

/**
 * Prints out the content of an array in a human readable format
 * 
 * @param m Array to visualize
 * @param n Length of the array
 * @param label Name of the array. If '\0', nothing will be printed
 */
void print_arr(double *m, size_t n, char label);

/**
 * Prints out the content of a square matrix in a human readable format
 * 
 * @param m Square matrix to visualize
 * @param n Dimension of the matrix
 * @param label Name of the matrix. If '\0', nothing will be printed
 */
void print_mat(double **m, size_t rows, size_t cols, char label);

/**
 * Prints out the content of a linearized square matrix in a human readable format
 * 
 * @param m Linearized square matrix to visualize
 * @param n Dimension of the matrix
 * @param label Name of the matrix. If '\0', nothing will be printed
 */
void print_mat_lin(double *m, size_t rows, size_t cols, char label);

/**
 * Compares two arrays to check if all the values are the same
 * 
 * @param a First array to check
 * @param b Second array to check
 * @param n Length of the arrays
 * 
 * @return Whether or not the two arrays contain the same values
 */
int compare_arr(double *a, double *b, size_t n);

/**
 * Compares two square matrices to check if all the values are the same
 * 
 * @param a First matrix to check
 * @param b Second matrix to check
 * @param n Dimension of both the matrices
 * 
 * @return Whether or not the two matrices contain the same values
 */
int compare_mat(double **a, double **b, size_t cols, size_t rows);

/**
 * Swaps the values pointed by the two pointers
 * 
 * @param a First pointer
 * @param b Second pointer
 */
void swap(double *a, double *b);

/**
 * Reads the values in a file and produces the corrisponding complete matrix
 * 
 * @param fname Name of the file
 * 
 * @return Complete matrix written in the file
 */
double *read_matrix_from_file(const char *fname);

/**
 * Checks if a function call caused an error based on its return valus
 * 
 * @param ret Return value of the function
 * @param mes Message to show if an error has occurred
 */
void check_ret(int ret, const char *mes);

#endif