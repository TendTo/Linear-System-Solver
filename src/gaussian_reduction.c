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
        for (int intern_row = row; intern_row < n; ++intern_row)
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
        for (int intern_row = row; intern_row < n; ++intern_row)
            sum -= x[intern_row] * U[row * n_complete + intern_row];

        x[row] = sum / U[row * n_complete + row];
    }
    //print_mat_lin(U, n, n_complete, 'U');
    free(U);
    return x;
}