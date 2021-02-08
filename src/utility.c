#include "utility.h"

double *create_complete_matrix_lin(double *A, double *b, size_t n)
{
    //Complete matrix
    double *U = (double *)malloc(sizeof(double) * (n * n + n));

    if (b)
    {
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                U[i * n + j + i] = A[i * n + j];
            }
            U[i * n + n + i] = b[i];
        }
    }
    else
    {
        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n + 1; ++j)
            {
                U[i * (n + 1) + j] = A[i * (n + 1) + j];
            }
        }
    }
    return U;
}

void print_arr(double *m, size_t n, char label)
{
    if (label != '\0')
        printf("Matrix %c: ", label);
    printf("[");
    for (size_t i = 0; i < n - 1; ++i)
        printf(" %.2f,", m[i]);

    printf(" %.2f ", m[n - 1]);
    printf("]\n");
}

void print_mat(double **m, size_t rows, size_t cols, char label)
{
    if (label != '\0')
        printf("Matrix %c: ", label);

    printf("[\n");
    for (size_t i = 0; i < rows; ++i)
    {
        printf("\t[");
        for (size_t j = 0; j < cols - 1; j++)
            printf(" %.2f,", m[i][j]);
        printf(" %.2f ", m[i][cols - 1]);
        printf("]\n");
    }
    printf("]\n");
}

void print_mat_lin(double *m, size_t rows, size_t cols, char label)
{
    if (label != '\0')
        printf("Matrix %c: ", label);

    printf("[\n");
    for (size_t i = 0; i < rows; ++i)
    {
        printf("\t[");
        for (size_t j = 0; j < cols - 1; j++)
            printf(" %.2f,", m[i * cols + j]);
        printf(" %.2f ", m[i * cols + cols - 1]);
        printf("]\n");
    }
    printf("]\n");
}

int compare_arr(double *a, double *b, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        double difference = a[i] - b[i];
        if (difference > PRECISION || difference < -PRECISION)
        {
#ifndef TEST
            fprintf(stderr, "[ERROR] %f != %f\n", a[i], b[i]);
#endif
            return 0;
        }
    }
    return 1;
}

int compare_mat(double **a, double **b, size_t rows, size_t cols)
{
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
        {
            double difference = a[i][j] - b[i][j];
            if (difference > PRECISION || difference < -PRECISION)
                return 0;
        }
    return 1;
}

void swap(double *a, double *b)
{
    double temp = *a;
    *a = *b;
    *b = temp;
}