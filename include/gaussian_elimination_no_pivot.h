/**
 * @file gaussian_elimination_no_pivot.h
 * @author TendTo (https://github.com/TendTo)
 * 
 * @brief Variatons of the gaussian elimination method
 * This file includes many variations of the gaussian elimination method, 
 * both serialized and parallel, to solve systems of linear equations
 */

#ifndef GAUSSIAN_ELIMINATION_NO_PIVOT_H
#define GAUSSIAN_ELIMINATION_NO_PIVOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "utility.h"
#include "ocl_boiler.h"

/**
 * Applies the Gaussian elimination with no pivoting.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_elimination_no_pivot(double *A, double *b, size_t n);

/**
 * Applies the Gaussian elimination with no pivoting using only one work-group so that all the matrix can be stored in local memory.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_elimination_no_pivot_gpu_lmem(double *A, double *b, size_t n, cl_status *status);

/**
 * Applies the Gaussian elimination with no pivoting that exploits the OpenCL textures.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
float *Gaussian_elimination_no_pivot_gpu_texture(float *A, float *b, size_t n, cl_status *status);

/**
 * Applies the Gaussian elimination with no pivoting that exploits the OpenCL textures meant to take advantage of vectorization.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
float *Gaussian_elimination_no_pivot_gpu_texture_vec(float *A, float *b, size_t n, cl_status *status);

/**
 * Applies the Gaussian elimination with no pivoting that lanches n kernels with the same 2 buffers.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_elimination_no_pivot_gpu_buffer(double *A, double *b, size_t n, cl_status *status);

/**
 * Applies the Gaussian elimination with no pivoting that lanches n kernels with the same 2 buffers 
 * meant to take advantage of vectorization.
 * This translates in a faster implementation, but there is the risk of being unable to solve otherwise solvable systems
 * 
 * @param A Matrix of coefficients of the equations
 * @param b Vector of know terms. If NULL, A will be assumed as a complete matrix of size (n X n + 1)
 * @param n Number of unknowns
 * @param status Pointer to a cl_status struct that represents the current status of the OpenCL runtime
 * 
 * @return Vector of length N containing the solutions to the system
 */
double *Gaussian_elimination_no_pivot_gpu_buffer_vec(double *A, double *b, size_t n, cl_status *status);

#endif