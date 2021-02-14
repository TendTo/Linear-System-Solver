/**
 * @file gaussian_elimination_pivot.h
 * @author TendTo (https://github.com/TendTo)
 * 
 * @brief Variatons of the gaussian elimination method
 * This file includes many variations of the gaussian elimination method, 
 * both serialized and parallel, to solve systems of linear equations
 */

#ifndef GAUSSIAN_ELIMINATION_PIVOT_H
#define GAUSSIAN_ELIMINATION_PIVOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utility.h"
#include "ocl_boiler.h"

/**
 * Applies the Gaussian elimination with partial pivoting.
 * Before appling the reductions, the best pivot is selected among the available ones if the same column
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_elimination_pivot(double *A, double *b, size_t n);

/**
 * Applies the Gaussian elimination with partial pivoting that exploits the OpenCL textures.
 * Before appling the reductions, the best pivot is selected among the available ones if the same column
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
float *Gaussian_elimination_pivot_gpu_texture(float *A, float *b, size_t n, cl_status *status);

/**
 * Applies the Gaussian elimination with partial pivoting that lanches n kernels with the same 2 buffers.
 * Before appling the reductions, the best pivot is selected among the available ones if the same column
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_elimination_pivot_gpu_buffer(double *A, double *b, size_t n, cl_status *status);

#endif