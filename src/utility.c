#include "utility.h"

float *d_to_f_array(double *a, size_t n)
{
    float *x = (float *)malloc(sizeof(float) * n);
    for (size_t i = 0; i < n; ++i)
        x[i] = (float)a[i];
    return x;
}

double *f_to_d_array(float *a, size_t n)
{
    double *x = (double *)malloc(sizeof(double) * n);
    for (size_t i = 0; i < n; ++i)
        x[i] = (double)a[i];
    return x;
}

double *generate_complete_matrix(int seed, int n)
{
    srand(seed);
    int tot = n * n + n;
    double *U = (double *)malloc(sizeof(double) * tot);
    for (int i = 0; i < tot; ++i)
        U[i] = (rand() / (double)RAND_MAX - 0.5) * 1000;
    return U;
}

float *generate_complete_matrix_f(int seed, int n)
{
    srand(seed);
    int tot = n * n + n;
    float *U = (float *)malloc(sizeof(float) * tot);
    for (int i = 0; i < tot; ++i)
        U[i] = (rand() / (float)RAND_MAX - 0.5) * 1000;
    return U;
}

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

float *create_complete_matrix_lin_f(float *A, float *b, size_t n)
{
    //Complete matrix
    float *U = (float *)malloc(sizeof(float) * (n * n + n));

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
        printf("Vector %c: ", label);
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

int compare_arr(double *a, const double *b, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        double difference = a[i] - b[i];
        if (difference > PRECISION || difference < -PRECISION || isnan(difference))
        {
#ifdef TEST
            fprintf(stderr, "[ERROR] at pos %ld: %f != %f\n", i, a[i], b[i]);
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

double *read_matrix_from_file(const char *fname)
{
    double *U;
    int cols, n_values, ret;

    FILE *in_file = fopen(fname, "r");
    if (!in_file)
        check_ret(-1, "opening file");
    fscanf(in_file, "%d", &cols);
    n_values = cols * cols + cols;
    U = (double *)malloc(sizeof(double) * n_values);
    memset(U, 0, n_values);
    for (size_t i = 0; i < n_values && ret != EOF; ++i)
        ret = fscanf(in_file, "%lf", &U[i]);

    ret = fclose(in_file);
    check_ret(ret, "closing file");
    return U;
}

double *read_matrix_n_from_file(const char *fname, int n)
{
    double *U;
    int n_values, ret;

    FILE *in_file = fopen(fname, "r");
    if (!in_file)
        check_ret(-1, "opening file");
    n_values = n * n + n;
    U = (double *)malloc(sizeof(double) * n_values);
    memset(U, 0, n_values);
    for (size_t i = 0; i < n_values && ret != EOF; ++i)
        ret = fscanf(in_file, "%lf", &U[i]);

    ret = fclose(in_file);
    check_ret(ret, "closing file");
    return U;
}

float *read_matrix_from_file_f(const char *fname)
{
    float *U;
    int cols, n_values, ret;

    FILE *in_file = fopen(fname, "r");
    if (!in_file)
        check_ret(-1, "opening file");
    fscanf(in_file, "%d", &cols);
    n_values = cols * cols + cols;
    U = (float *)malloc(sizeof(float) * n_values);
    memset(U, 0, n_values);
    for (size_t i = 0; i < n_values && ret != EOF; ++i)
        ret = fscanf(in_file, "%f", &U[i]);

    ret = fclose(in_file);
    check_ret(ret, "closing file");
    return U;
}

void check_ret(int ret, const char *mes)
{
    if (ret == -1)
    {
        perror(mes);
        exit(1);
    }
}