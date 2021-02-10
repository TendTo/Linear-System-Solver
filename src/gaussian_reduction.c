#include "gaussian_reduction.h"

double *Gaussian_reduction_no_pivot(double *A, double *b, size_t n)
{
    size_t n_complete = n + 1;
    double *U = create_complete_matrix_lin(A, b, n);
    double *x = (double *)malloc(sizeof(double) * n);
    memset(x, 0, sizeof(double) * n);

    //Alter the A and B matrices to solve the equation
    for (size_t pivot = 0; pivot < n - 1; ++pivot)
    {
        for (size_t row = pivot + 1; row < n; ++row)
        {
            double mult = -U[row * n_complete + pivot] / U[pivot * n_complete + pivot];
            U[row * n_complete + pivot] = 0;
            for (size_t col = pivot + 1; col < n_complete; ++col)
            {
                U[row * n_complete + col] += U[pivot * n_complete + col] * mult;
            }
        }
    }

    //After the A matrix is made into a triangular matrix, solve the simplified system
    for (int row = n - 1; row >= 0; --row)
    {
        double sum = U[row * n_complete + n];

        //Adjust the current sum by subtracting the values already found
        for (int intern_row = row + 1; intern_row < n; ++intern_row)
            sum -= x[intern_row] * U[row * n_complete + intern_row];

        x[row] = sum / U[row * n_complete + row];
    }
    //print_mat_lin(U, n, n_complete, 'U');
    free(U);
    return x;
}

double *Gaussian_reduction_pivot(double *A, double *b, size_t n)
{
    size_t n_complete = n + 1;
    double *U = create_complete_matrix_lin(A, b, n);
    double *x = (double *)malloc(sizeof(double) * n);
    memset(x, 0, sizeof(double) * n);

    for (size_t i = 0; i < n - 1; ++i)
    {

        size_t pivot = i;
        double maxPivot = 0.0;

        //Find the best pivot
        for (size_t ii = i; ii < n; ++ii)
        {
            if (fabs(U[ii * n_complete + i]) > maxPivot)
            {
                maxPivot = fabs(U[ii * n_complete + i]);
                pivot = ii;
            }
        }
        //Check if the system is still solvable
        // if (fabs(maxPivot) < 0.01f)

        //If the pivot was found in another row, invert the two rows
        if (pivot != i)
        {
            for (size_t ii = 0; ii < n_complete; ++ii)
                swap(&U[pivot * n_complete + ii], &U[i * n_complete + ii]);
        }
        pivot = i;
        for (size_t row = pivot + 1; row < n; ++row)
        {
            double mult = -U[row * n_complete + pivot] / U[pivot * n_complete + pivot];
            U[row * n_complete + pivot] = 0.0;
            for (size_t col = pivot + 1; col < n_complete; ++col)
            {
                U[row * n_complete + col] += U[pivot * n_complete + col] * mult;
            }
        }
    }

    //After the A matrix is made into a triangular matrix, solve the simplified system
    for (int row = n - 1; row >= 0; --row)
    {
        double sum = U[row * n_complete + n];

        //Adjust the current sum by subtracting the values already found
        for (int intern_row = row + 1; intern_row < n; ++intern_row)
            sum -= x[intern_row] * U[row * n_complete + intern_row];

        x[row] = sum / U[row * n_complete + row];
    }
    //print_mat_lin(U, n, n_complete, 'U');
    free(U);
    return x;
}

double *Gaussian_reduction_no_pivot_gpu_naive(double *A, double *b, size_t n, cl_status *status)
{
    if (!status)
    {
        fprintf(stderr, "[error] null status");
        return NULL;
    }
    cl_int rows = n, cols = n + 1, err;
    size_t U_memsize = sizeof(double) * rows * cols;
    size_t x_memsize = sizeof(double) * rows;

    double *h_U = NULL, *h_x = (double *)malloc(x_memsize);
    if (b)
        h_U = create_complete_matrix_lin(A, b, n);
    else
        h_U = A;

    //Transfer complete matrix to device
    cl_mem d_U = clCreateBuffer(status->ctx, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, U_memsize, h_U, &err);
    ocl_check(err, "create input buffer");
    cl_mem d_x = clCreateBuffer(status->ctx, CL_MEM_WRITE_ONLY, x_memsize, NULL, &err);
    ocl_check(err, "create output buffer");

    //Create kernel
    cl_kernel init_mat_k = clCreateKernel(status->prog, "gaussian_reduction_no_pivot_naive", &err);
    ocl_check(err, "create gaussian_reduction_naive kernel");

    size_t gws[] = {cols};
    size_t lws[] = {cols}; //To make sure there is only one workgroup

    err = clSetKernelArg(init_mat_k, 0, sizeof(cl_int), &cols);
    ocl_check(err, "set arg 0 for init_mat_k");
    err = clSetKernelArg(init_mat_k, 1, sizeof(d_U), &d_U);
    ocl_check(err, "set arg 1 for init_mat_k");
    err = clSetKernelArg(init_mat_k, 2, sizeof(d_x), &d_x);
    ocl_check(err, "set arg 2 for init_mat_k");
    err = clSetKernelArg(init_mat_k, 3, U_memsize + x_memsize, NULL);
    ocl_check(err, "set arg 3 for init_mat_k");

    cl_event gaussian_evt, read_evt, unmap_evt;

    //Launch the kernel
    err = clEnqueueNDRangeKernel(status->que, init_mat_k,
                                 1, NULL, gws, lws,
                                 0, NULL, &gaussian_evt);
    ocl_check(err, "enqueue gaussian_reduction");

#if 1
    err = clEnqueueReadBuffer(status->que, d_x, CL_TRUE,
                              0, x_memsize, h_x,
                              1, &gaussian_evt, &read_evt);
    ocl_check(err, "enqueue read buffer");

#else
    //Map the buffer on the host so it can be read (a buffer read may be better to keep the result on the host)
    h_x = (double *)clEnqueueMapBuffer(status->que, d_x, CL_TRUE, CL_MAP_READ,
                                       0, x_memsize, 1,
                                       &gaussian_evt, &read_evt, &err);
    ocl_check(err, "enqueue map buffer");
    print_arr(h_x, 10, 'x');
    //Unmap the previously mapped region
    err = clEnqueueUnmapMemObject(status->que, d_x, h_x, 1, &read_evt, &unmap_evt);
    ocl_check(err, "enqueue unmap buffer");

    clWaitForEvents(1, &unmap_evt);
#endif
    if (b)
        free(h_U);
    clReleaseKernel(init_mat_k);
    clReleaseMemObject(d_U);
    clReleaseMemObject(d_x);
#if TEST
    cl_ulong init_evt_rn = runtime_ns(gaussian_evt);
    cl_ulong read_evt_rn = runtime_ns(read_evt);

    printf("-----\nGaussian_evt runtime:\t%lu ns\n", init_evt_rn);
    printf("Read_evt runtime:\t%lu ns\n", read_evt_rn);
#if 0
    cl_ulong unmap_evt_rn = runtime_ns(unmap_evt);
    printf("Unmap_evt runtime:\t%lu ns\n", unmap_evt_rn);
#endif
    printf("-----\n");
#endif
    return h_x;
}