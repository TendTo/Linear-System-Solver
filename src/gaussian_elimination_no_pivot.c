#include "gaussian_elimination_no_pivot.h"

double *Gaussian_elimination_no_pivot(double *A, double *b, size_t n)
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

double *Gaussian_elimination_no_pivot_gpu_lmem(double *A, double *b, size_t n, cl_status *status)
{
    if (!status)
    {
        fprintf(stderr, "[ERROR] null status");
        return NULL;
    }
    cl_int rows = n, cols = n + 1, err;
    size_t U_memsize = sizeof(cl_double) * rows * cols;
    size_t x_memsize = sizeof(cl_double) * rows;
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
    cl_kernel init_mat_k = clCreateKernel(status->prog, "gaussian_elimination_no_pivot_lmem", &err);
    ocl_check(err, "create gaussian_elimination_naive kernel");

    size_t gws[] = {cols};
    size_t lws[] = {cols}; //To make sure there is only one workgroup

    err = clSetKernelArg(init_mat_k, 0, sizeof(cl_int), &cols);
    ocl_check(err, "set arg 0 for init_mat_k");
    err = clSetKernelArg(init_mat_k, 1, sizeof(d_U), &d_U);
    ocl_check(err, "set arg 1 for init_mat_k");
    err = clSetKernelArg(init_mat_k, 2, sizeof(d_x), &d_x);
    ocl_check(err, "set arg 2 for init_mat_k");
    err = clSetKernelArg(init_mat_k, 3, U_memsize + 2 * x_memsize, NULL);
    ocl_check(err, "set arg 3 for init_mat_k");

    cl_event gaussian_evt, read_evt;

    //Launch the kernel
    err = clEnqueueNDRangeKernel(status->que, init_mat_k,
                                 1, NULL, gws, lws,
                                 0, NULL, &gaussian_evt);
    ocl_check(err, "enqueue gaussian_elimination");

#if 1
    err = clEnqueueReadBuffer(status->que, d_x, CL_TRUE,
                              0, x_memsize, h_x,
                              1, &gaussian_evt, &read_evt);
    ocl_check(err, "enqueue read buffer");

#else
    cl_event unmap_evt;
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
    cl_ulong gaussian_evt_rn = runtime_ns(gaussian_evt);
    cl_ulong read_evt_rn = runtime_ns(read_evt);

    printf("-----\n");
    printf("GPU no pivot lmem | n: %ld\n", n);
    printf("Gaussian_evt:\truntime %lu ns\t%.4g GE/s\t%.4g GB/s\n",
           gaussian_evt_rn, (rows * cols + rows) / (double)gaussian_evt_rn, (U_memsize + x_memsize) / (double)gaussian_evt_rn);
    printf("Read_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           read_evt_rn, rows / (double)read_evt_rn, x_memsize / (double)read_evt_rn);
#if 0
    cl_ulong unmap_evt_rn = runtime_ns(unmap_evt);
    printf("Unmap_evt runtime:\t%lu ns\n", unmap_evt_rn);
#endif
    printf("-----\n");
#endif
    return h_x;
}

float *Gaussian_elimination_no_pivot_gpu_texture(float *A, float *b, size_t n, cl_status *status)
{
    if (!status)
    {
        fprintf(stderr, "[ERROR] null status");
        return NULL;
    }
    cl_int rows = n, cols = n + 1, gaussian_wi = 0, err;
    size_t U_memsize = sizeof(cl_float) * rows * cols;
    size_t x_memsize = sizeof(cl_float) * rows;

    float *h_U = NULL, *h_x = (float *)malloc(x_memsize);
    if (b)
        h_U = create_complete_matrix_lin_f(A, b, n);
    else
        h_U = A;

    // Texture settings
    cl_image_format tex_fmt = {.image_channel_order = CL_R,
                               .image_channel_data_type = CL_FLOAT};
    cl_image_desc tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));
    tex_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    tex_desc.image_width = cols;
    tex_desc.image_height = rows;
    tex_desc.image_depth = 1;
    const size_t origin[] = {0, 0, 0};
    const size_t region[] = {cols, rows, 1};

    //Create memory objects
    cl_mem d_even_U = clCreateImage(status->ctx, CL_MEM_READ_WRITE,
                                    &tex_fmt, &tex_desc, NULL, &err);
    ocl_check(err, "create input tex 1");
    cl_mem d_odd_U = clCreateImage(status->ctx, CL_MEM_READ_WRITE,
                                   &tex_fmt, &tex_desc, NULL, &err);
    ocl_check(err, "create input tex 2");
    cl_mem d_x = clCreateBuffer(status->ctx, CL_MEM_WRITE_ONLY, x_memsize, NULL, &err);
    ocl_check(err, "create output buffer");

    //Create kernel
    cl_kernel gaussian_k = clCreateKernel(status->prog, "gaussian_elimination_no_pivot_texture", &err);
    ocl_check(err, "create gaussian_elimination_no_pivot_texture kernel");
    cl_kernel solve_k = clCreateKernel(status->prog, "gaussian_elimination_solve_texture", &err);
    ocl_check(err, "create gaussian_elimination_no_pivot_texture kernel");

    //Kernel settings
    size_t gws[] = {cols - 1, rows - 1};
    size_t lws[] = {16, 16};

    cl_event upload_evt, gaussian_evt, solve_evt, read_evt;
    cl_ulong gaussian_evt_rn = 0, solve_evt_rn = 0, read_evt_rn = 0;

    //Enqueue the events
    err = clEnqueueWriteImage(status->que, d_even_U, CL_TRUE,
                              origin, region, 0, 0, h_U,
                              0, NULL, &upload_evt);
    ocl_check(err, "enqueue upload tex");

    cl_mem d_in_U = d_even_U, d_out_U = d_odd_U;
    for (int i = 0; i < rows - 1; ++i)
    {
        //Reduce the number of workitems
        gws[0] = round_mul_up(cols - 1 - i, lws[0]);
        gws[1] = round_mul_up(rows - 1 - i, lws[1]);
        gaussian_wi += gws[0] * gws[1]; // Number of work-item in the kernel

        err = clSetKernelArg(gaussian_k, 0, sizeof(cl_int), &i);
        ocl_check(err, "set arg 0 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 1, sizeof(d_in_U), &d_in_U);
        ocl_check(err, "set arg 1 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 2, sizeof(d_out_U), &d_out_U);
        ocl_check(err, "set arg 2 for gaussian_k");

        err = clEnqueueNDRangeKernel(status->que, gaussian_k,
                                     2, NULL, gws, lws,
                                     1, &upload_evt, &gaussian_evt);
        ocl_check(err, "enqueue gaussian_k");

        //Swap the tex
        cl_mem d_temp = d_in_U;
        d_in_U = d_out_U;
        d_out_U = d_temp;

        clWaitForEvents(1, &gaussian_evt);
        gaussian_evt_rn += runtime_ns(gaussian_evt);
    }
    gws[0] = round_mul_up(rows, 16);
    lws[0] = round_mul_up(rows, 16);

    err = clSetKernelArg(solve_k, 0, sizeof(d_even_U), &d_even_U);
    ocl_check(err, "set arg 0 for solve_k");
    err = clSetKernelArg(solve_k, 1, sizeof(d_odd_U), &d_odd_U);
    ocl_check(err, "set arg 1 for solve_k");
    err = clSetKernelArg(solve_k, 2, sizeof(d_x), &d_x);
    ocl_check(err, "set arg 2 for solve_k");
    err = clSetKernelArg(solve_k, 3, x_memsize * 2, NULL);
    ocl_check(err, "set arg 3 for solve_k");

    err = clEnqueueNDRangeKernel(status->que, solve_k,
                                 1, NULL, gws, lws,
                                 1, &gaussian_evt, &solve_evt);
    ocl_check(err, "enqueue solve_k");

#if 1
    err = clEnqueueReadBuffer(status->que, d_x, CL_TRUE,
                              0, x_memsize, h_x,
                              1, &solve_evt, &read_evt);
    ocl_check(err, "enqueue read buffer");

    //print_arr(h_x, n, 'x');
#else
    //Map the buffer on the host so it can be read (a buffer read may be better to keep the result on the host)
    cl_event unmap_evt;
    h_x = (float *)clEnqueueMapBuffer(status->que, d_x, CL_TRUE, CL_MAP_READ,
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
    clReleaseKernel(gaussian_k);
    clReleaseKernel(solve_k);
    clReleaseMemObject(d_even_U);
    clReleaseMemObject(d_odd_U);
    clReleaseMemObject(d_x);
#if TEST
    solve_evt_rn = runtime_ns(solve_evt);
    read_evt_rn = runtime_ns(read_evt);
    printf("-----\n");
    printf("GPU no pivot tex | n: %ld\n", n);
    printf("Gaussian_evt:\truntime %lu ns\t%.4g GE/s\t%.4g GB/s\n",
           gaussian_evt_rn, (2.0 * gaussian_wi) / gaussian_evt_rn, (2.0 * gaussian_wi * sizeof(cl_float)) / gaussian_evt_rn);
    printf("Solve_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           solve_evt_rn, (rows * cols / 2.0 + rows) / solve_evt_rn, (U_memsize / 2.0 + x_memsize) / solve_evt_rn);
    printf("Read_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           read_evt_rn, rows / (float)read_evt_rn, x_memsize / (float)read_evt_rn);
#if 0
    cl_ulong unmap_evt_rn = runtime_ns(unmap_evt);
    printf("Unmap_evt runtime:\t%lu ns\n", unmap_evt_rn);
#endif
    printf("-----\n");
#endif
    return h_x;
}

float *Gaussian_elimination_no_pivot_gpu_texture_4(float *A, float *b, size_t n, cl_status *status)
{
    if (!status)
    {
        fprintf(stderr, "[ERROR] null status");
        return NULL;
    }
    if ((n + 1) & 3)
    {
        fprintf(stderr, "[ERROR] not divisible by 4");
        return NULL;
    }
    cl_int rows = n, cols = (n + 1) / 4, gaussian_wi = 0, err;
    size_t U_memsize = sizeof(cl_float) * rows * cols * 4;
    size_t x_memsize = sizeof(cl_float) * rows;

    float *h_U = NULL, *h_x = (float *)malloc(x_memsize);
    if (b)
        h_U = create_complete_matrix_lin_f(A, b, n);
    else
        h_U = A;

    // Texture settings
    cl_image_format tex_fmt = {.image_channel_order = CL_RGBA,
                               .image_channel_data_type = CL_FLOAT};
    cl_image_desc tex_desc;
    memset(&tex_desc, 0, sizeof(tex_desc));
    tex_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
    tex_desc.image_width = cols;
    tex_desc.image_height = rows;
    tex_desc.image_depth = 1;
    const size_t origin[] = {0, 0, 0};
    const size_t region[] = {cols, rows, 1};

    //Create memory objects
    cl_mem d_even_U = clCreateImage(status->ctx, CL_MEM_READ_WRITE,
                                    &tex_fmt, &tex_desc, NULL, &err);
    ocl_check(err, "create input tex 1");
    cl_mem d_odd_U = clCreateImage(status->ctx, CL_MEM_READ_WRITE,
                                   &tex_fmt, &tex_desc, NULL, &err);
    ocl_check(err, "create input tex 2");
    cl_mem d_x = clCreateBuffer(status->ctx, CL_MEM_WRITE_ONLY, x_memsize, NULL, &err);
    ocl_check(err, "create output buffer");

    //Create kernel
    cl_kernel gaussian_k = clCreateKernel(status->prog, "gaussian_elimination_no_pivot_texture_4", &err);
    ocl_check(err, "create gaussian_elimination_no_pivot_texture kernel");
    cl_kernel solve_k = clCreateKernel(status->prog, "gaussian_elimination_solve_texture_4", &err);
    ocl_check(err, "create gaussian_elimination_no_pivot_texture kernel");

    //Kernel settings
    size_t gws[] = {cols - 1, rows - 1};
    size_t lws[] = {16, 16};

    cl_event upload_evt, gaussian_evt, solve_evt, read_evt;
    cl_ulong gaussian_evt_rn = 0, solve_evt_rn = 0, read_evt_rn = 0;

    //Enqueue the events
    err = clEnqueueWriteImage(status->que, d_even_U, CL_TRUE,
                              origin, region, 0, 0, h_U,
                              0, NULL, &upload_evt);
    ocl_check(err, "enqueue upload tex");

    cl_mem d_in_U = d_even_U, d_out_U = d_odd_U;
    for (int i = 0; i < rows - 1; ++i)
    {
        //Reduce the number of workitems
        gws[0] = round_mul_up(cols - i / 4, lws[0]);
        gws[1] = round_mul_up(rows - 1 - i, lws[1]);
        gaussian_wi += gws[0] * gws[1]; // Number of work-item in the kernel

        err = clSetKernelArg(gaussian_k, 0, sizeof(cl_int), &i);
        ocl_check(err, "set arg 0 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 1, sizeof(d_in_U), &d_in_U);
        ocl_check(err, "set arg 1 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 2, sizeof(d_out_U), &d_out_U);
        ocl_check(err, "set arg 2 for gaussian_k");

        err = clEnqueueNDRangeKernel(status->que, gaussian_k,
                                     2, NULL, gws, lws,
                                     1, &upload_evt, &gaussian_evt);
        ocl_check(err, "enqueue gaussian_k");

        //Swap the tex
        cl_mem d_temp = d_in_U;
        d_in_U = d_out_U;
        d_out_U = d_temp;

        clWaitForEvents(1, &gaussian_evt);
        gaussian_evt_rn += runtime_ns(gaussian_evt);
    }
    gws[0] = round_mul_up(rows, 16);
    lws[0] = round_mul_up(rows, 16);

    err = clSetKernelArg(solve_k, 0, sizeof(d_even_U), &d_even_U);
    ocl_check(err, "set arg 0 for solve_k");
    err = clSetKernelArg(solve_k, 1, sizeof(d_odd_U), &d_odd_U);
    ocl_check(err, "set arg 1 for solve_k");
    err = clSetKernelArg(solve_k, 2, sizeof(d_x), &d_x);
    ocl_check(err, "set arg 2 for solve_k");
    err = clSetKernelArg(solve_k, 3, x_memsize * 2, NULL);
    ocl_check(err, "set arg 3 for solve_k");

    err = clEnqueueNDRangeKernel(status->que, solve_k,
                                 1, NULL, gws, lws,
                                 1, &gaussian_evt, &solve_evt);
    ocl_check(err, "enqueue solve_k");

#if 1
    err = clEnqueueReadBuffer(status->que, d_x, CL_TRUE,
                              0, x_memsize, h_x,
                              1, &solve_evt, &read_evt);
    ocl_check(err, "enqueue read buffer");

    //print_arr(h_x, n, 'x');
#else
    //Map the buffer on the host so it can be read (a buffer read may be better to keep the result on the host)
    cl_event unmap_evt;
    h_x = (float *)clEnqueueMapBuffer(status->que, d_x, CL_TRUE, CL_MAP_READ,
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
    clReleaseKernel(gaussian_k);
    clReleaseKernel(solve_k);
    clReleaseMemObject(d_even_U);
    clReleaseMemObject(d_odd_U);
    clReleaseMemObject(d_x);
#if TEST
    solve_evt_rn = runtime_ns(solve_evt);
    read_evt_rn = runtime_ns(read_evt);
    printf("-----\n");
    printf("GPU no pivot tex vec 4 | n: %ld\n", n);
    printf("Gaussian_evt:\truntime %lu ns\t%.4g GE/s\t%.4g GB/s\n",
           gaussian_evt_rn, (8.0 * gaussian_wi) / gaussian_evt_rn, (8.0 * gaussian_wi * sizeof(cl_float)) / gaussian_evt_rn);
    printf("Solve_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           solve_evt_rn, (rows * cols * 4 / 2.0 + rows) / solve_evt_rn, (U_memsize / 2.0 + x_memsize) / solve_evt_rn);
    printf("Read_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           read_evt_rn, rows / (float)read_evt_rn, x_memsize / (float)read_evt_rn);
#if 0
    cl_ulong unmap_evt_rn = runtime_ns(unmap_evt);
    printf("Unmap_evt runtime:\t%lu ns\n", unmap_evt_rn);
#endif
    printf("-----\n");
#endif
    return h_x;
}

double *Gaussian_elimination_no_pivot_gpu_buffer(double *A, double *b, size_t n, cl_status *status)
{
    if (!status)
    {
        fprintf(stderr, "[ERROR] null status");
        return NULL;
    }
    cl_int rows = n, cols = n + 1, gaussian_wi = 0, err;
    size_t U_memsize = sizeof(cl_double) * rows * cols;
    size_t x_memsize = sizeof(cl_double) * rows;

    double *h_U = NULL, *h_x = (double *)malloc(x_memsize);
    if (b)
        h_U = create_complete_matrix_lin(A, b, n);
    else
        h_U = A;

    //Create memory objects
    cl_mem d_even_U = clCreateBuffer(status->ctx, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, U_memsize, h_U, &err);
    ocl_check(err, "create even buffer");
    cl_mem d_odd_U = clCreateBuffer(status->ctx, CL_MEM_READ_WRITE, U_memsize, NULL, &err);
    ocl_check(err, "create odd buffer");
    cl_mem d_x = clCreateBuffer(status->ctx, CL_MEM_WRITE_ONLY, x_memsize, NULL, &err);
    ocl_check(err, "create output buffer");

    //Create kernel
    cl_kernel gaussian_k = clCreateKernel(status->prog, "gaussian_elimination_no_pivot_buffer", &err);
    ocl_check(err, "create gaussian_elimination_no_pivot_buffer kernel");
    cl_kernel solve_k = clCreateKernel(status->prog, "gaussian_elimination_solve_buffer", &err);
    ocl_check(err, "create gaussian_elimination_solve_buffer kernel");

    //Kernel settings
    size_t gws[] = {cols - 1, rows - 1};
    size_t lws[] = {16, 16};

    cl_event gaussian_evt, solve_evt, read_evt;
    cl_ulong gaussian_evt_rn = 0, solve_evt_rn = 0, read_evt_rn = 0;

    cl_mem d_in_U = d_even_U, d_out_U = d_odd_U;
    //Enqueue the events
    for (int i = 0; i < rows - 1; ++i)
    {
        //Reduce the number of workitems
        gws[0] = round_mul_up(cols - 1 - i, lws[0]);
        gws[1] = round_mul_up(rows - 1 - i, lws[1]);
        gaussian_wi += gws[0] * gws[1]; // Number of work-item in the kernel

        err = clSetKernelArg(gaussian_k, 0, sizeof(cl_int), &rows);
        ocl_check(err, "set arg 0 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 1, sizeof(cl_int), &cols);
        ocl_check(err, "set arg 1 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 2, sizeof(cl_int), &i);
        ocl_check(err, "set arg 2 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 3, sizeof(d_in_U), &d_in_U);
        ocl_check(err, "set arg 3 for gaussian_k");
        err = clSetKernelArg(gaussian_k, 4, sizeof(d_out_U), &d_out_U);
        ocl_check(err, "set arg 4 for gaussian_k");

        err = clEnqueueNDRangeKernel(status->que, gaussian_k,
                                     2, NULL, gws, lws,
                                     0, NULL, &gaussian_evt);
        ocl_check(err, "enqueue gaussian_k");

        //Swap the tex
        cl_mem d_temp = d_in_U;
        d_in_U = d_out_U;
        d_out_U = d_temp;

        clWaitForEvents(1, &gaussian_evt);
        gaussian_evt_rn += runtime_ns(gaussian_evt);
    }
    gws[0] = round_mul_up(rows, 16);
    lws[0] = round_mul_up(rows, 16);

    err = clSetKernelArg(solve_k, 0, sizeof(cl_int), &rows);
    ocl_check(err, "set arg 0 for solve_k");
    err = clSetKernelArg(solve_k, 1, sizeof(cl_int), &cols);
    ocl_check(err, "set arg 1 for solve_k");
    err = clSetKernelArg(solve_k, 2, sizeof(d_even_U), &d_even_U);
    ocl_check(err, "set arg 2 for solve_k");
    err = clSetKernelArg(solve_k, 3, sizeof(d_odd_U), &d_odd_U);
    ocl_check(err, "set arg 3 for solve_k");
    err = clSetKernelArg(solve_k, 4, sizeof(d_x), &d_x);
    ocl_check(err, "set arg 4 for solve_k");
    err = clSetKernelArg(solve_k, 5, x_memsize * 2, NULL);
    ocl_check(err, "set arg 5 for solve_k");

    err = clEnqueueNDRangeKernel(status->que, solve_k,
                                 1, NULL, gws, lws,
                                 1, &gaussian_evt, &solve_evt);
    ocl_check(err, "enqueue solve_k");

#if 1
    err = clEnqueueReadBuffer(status->que, d_x, CL_TRUE,
                              0, x_memsize, h_x,
                              1, &solve_evt, &read_evt);
    ocl_check(err, "enqueue read buffer");

    //print_arr(h_x, n, 'x');
#else
    //Map the buffer on the host so it can be read (a buffer read may be better to keep the result on the host)
    cl_event unmap_evt;
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
    clReleaseKernel(gaussian_k);
    clReleaseKernel(solve_k);
    clReleaseMemObject(d_even_U);
    clReleaseMemObject(d_odd_U);
    clReleaseMemObject(d_x);
#if TEST
    solve_evt_rn = runtime_ns(solve_evt);
    read_evt_rn = runtime_ns(read_evt);
    printf("-----\n");
    printf("GPU no pivot buffer | n: %ld\n", n);
    printf("Gaussian_evt:\truntime %lu ns\t%.4g GE/s\t%.4g GB/s\n",
           gaussian_evt_rn, (2.0 * gaussian_wi) / gaussian_evt_rn, (2.0 * gaussian_wi * sizeof(cl_double)) / gaussian_evt_rn);
    printf("Solve_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           solve_evt_rn, (rows * cols / 2.0 + rows) / solve_evt_rn, (U_memsize / 2.0 + x_memsize) / solve_evt_rn);
    printf("Read_evt:\truntime %lu ns  \t%.4g GE/s\t%.4g GB/s\n",
           read_evt_rn, rows / (double)read_evt_rn, x_memsize / (double)read_evt_rn);
#if 0
    cl_ulong unmap_evt_rn = runtime_ns(unmap_evt);
    printf("Unmap_evt runtime:\t%lu ns\n", unmap_evt_rn);
#endif
    printf("-----\n");
#endif
    return h_x;
}