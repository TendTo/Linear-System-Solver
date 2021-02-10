/**
 * @file ocl_boiler.h
 * @author Giuseppe Bilotta (https://github.com/Oblomov)
 * 
 * @brief OpenCL boilerplate
 * A collection of functions wrapping the most common boilerplate 
 * of an OpenCL program.
 */

#ifndef OCL_BOILER
#define OCL_BOILER

#if 0 // example usage:
#include "ocl_boiler.h"

int main(int argc, char *argv[])
{
	cl_platform_id p = select_platform();
	cl_device_id d = select_device(p);
	cl_context ctx = create_context(p, d);
	cl_command_queue que = create_queue(ctx, d);
	cl_program prog = create_program("kernels.ocl", ctx, d);

	/* Here starts the custom part: extract kernels,
	 * allocate buffers, run kernels, get results */

	return 0;
}
#endif

#define CL_TARGET_OPENCL_VERSION 120 // Using version 1.2 of the OpenCL library

/* Include the headers defining the OpenCL host API */
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define BUFSIZE 4096

/**
 * @struct cl_status
 * @brief This structure contains all the initialized info neede to run an OpenCL applicative
 */
typedef struct
{
	cl_platform_id p;
	cl_device_id d;
	cl_context ctx;
	cl_command_queue que;
	cl_program prog;
} cl_status;

/** 
 * Check an OpenCL error status, printing a message and exiting
 * in case of failure
 * 
 * @param err Error code returned from a OpenCL function
 * @param msg String that will be shown if an error has occurred
 */
void ocl_check(cl_int err, const char *msg, ...);

/**
 *  Return the ID of the platform specified in the OCL_PLATFORM
 * 
 * @return The ID of the platform specified in the OCL_PLATFORM
 */
cl_platform_id select_platform();

/**
 * Return the ID of the device (of the given platform p) specified in the
 * OCL_DEVICE environment variable (or the first one if none specified)
 * 
 * @see select_platform()
 * @param p ID of the platform selected with select_platform()
 * 
 * @return The ID of the device specified in OCL_DEVICE. Defaults to chose the first one
 */
cl_device_id select_device(cl_platform_id p);

/**
 * Create a one-device context
 * 
 * @see select_platform()
 * @see select_device()
 * @param p ID of the platform selected with select_platform()
 * @param d ID of the device selected with select_device()
 * 
 * @return The OpenCL context with the chosen platform and device
 */
cl_context create_context(cl_platform_id p, cl_device_id d);

/**
 * Create a command queue for the given device in the given context
 * 
 * @see create_context()
 * @see select_device()
 * @param ctx OpenCL context created with create_context()
 * @param d ID of the device selected with select_device()
 * 
 * @return A new command queue for the chosen device in the given context
 */
cl_command_queue create_queue(cl_context ctx, cl_device_id d);

/**
 * Compile the device part of the program, stored in the external
 * file `fname`, for device `dev` in context `ctx`
 * 
 * @see create_context()
 * @see select_device()
 * @param fname Name of the file that contains the OpenCL program
 * @param ctx OpenCL context created with create_context()
 * @param d ID of the device selected with select_device()
 * 
 * @return A new OpenCL program generated from the given file
 */
cl_program create_program(const char *const fname, cl_context ctx, cl_device_id d);

/**
 * Exectute all the needed function calls to produce a cl_status struct
 * 
 * @see select_platform()
 * @see select_device()
 * @see create_context()
 * @see create_queue()
 * @see create_program()
 * @param status Pointer to a cl_status struct that will be filled with the exectution of the function
 * @param fname Name of the file that contains the OpenCL program
 */
void clCreateStatus(cl_status *status, const char *const fname);

/**
 * Free all the resources used in the cl_struct
 * 
 * @see create_cl_status()
 * @param status Pointer to a cl_status struct that will be filled with all the resources to free
 */
void clFreeStatus(cl_status *status);

/**
 * Runtime of an event, in nanoseconds. Note that if NS is the
 * runtimen of an event in nanoseconds and NB is the number of byte
 * read and written during the event, NB/NS is the effective bandwidth
 * expressed in GB/s
 * 
 * @param evt Event for which you want to calculate the runtime
 * 
 * @return Runtime in ns
 */
cl_ulong runtime_ns(cl_event evt);
/**
 * Total runtime enlapsed between two events, in nanoseconds
 * 
 * @param from Starting event
 * @param to Ending event
 * 
 * @return Enlapsed time between the two events, in ns
 */
cl_ulong total_runtime_ns(cl_event from, cl_event to);

/**
 * Runtime of an event, in milliseconds
 * 
 * @param evt Event for which you want to calculate the runtime
 * 
 * @return Runtime in ms
 */
double runtime_ms(cl_event evt);

/**
 * Total runtime enlapsed between two events, in milliseconds
 * 
 * @param from Starting event
 * @param to Ending event
 * 
 * @return Enlapsed time between the two events, in ms
 */
double total_runtime_ms(cl_event from, cl_event to);

/**
 * Round the gws to the next multiple of the lws
 * 
 * @param gws Group Work Size, the total number of workitem for that dimension
 * @param lws Local Work Size, the number of workitem that will make each group
 * 
 * @return Smaller multiple of lws greater or equal to gws
 */
size_t round_mul_up(size_t gws, size_t lws);

#endif