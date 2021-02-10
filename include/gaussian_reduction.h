/**
 * @file gaussian_reduction.h
 * @author TendTo (https://github.com/TendTo)
 * 
 * @brief Variatons of the gaussian reduction method
 * This file includes many variations of the gaussian reduction method, 
 * both serialized and parallel, to solve systems of linear equations
 */

#ifndef SERIAL_GAUSSIAN_REDUCTION_H
#define SERIAL_GAUSSIAN_REDUCTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utility.h"
#include "ocl_boiler.h"

/**
 * Applies the Gaussian reduction with no pivoting.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_reduction_no_pivot(double *A, double *b, size_t n);

/**
 * Applies the Gaussian reduction with partial pivoting.
 * Befor appling the reductions, the best pivot is selected among the available ones
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_reduction_pivot(double *A, double *b, size_t n);

/**
 * Applies the Gaussian reduction with no pivoting.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_reduction_no_pivot_gpu_naive(double *A, double *b, size_t n, cl_status *status);

#endif