#include "main.h"

int main(int argc, char *const *argv)
{
    int opt, seed = 123, n = 1000, impcode = 0, vectorize = 0;
    char infile[256], outfile[256], oclfile[512];
    infile[0] = '\0';
    outfile[0] = '\0';

    //Handle inputs
    while ((opt = getopt(argc, argv, ":hvi:o:s:d:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            strncpy(infile, optarg, 255);
            infile[255] = '\0';
            break;
        case 'o':
            strncpy(outfile, optarg, 255);
            outfile[255] = '\0';
            break;
        case 's':
            seed = atoi(optarg);
            if (seed < 1)
            {
                fprintf(stderr, "%s: -%c value must be greater than 0\n", argv[0], opt);
                exit(EXIT_FAILURE);
            }
            break;
        case 'd':
            n = atoi(optarg);
            if (n < 1)
            {
                fprintf(stderr, "%s: -%c value must be greater than 0\n", argv[0], opt);
                exit(EXIT_FAILURE);
            }
            break;
        case 'h':
            printf(HELP_TOOLTIP, argv[0]);
            exit(EXIT_SUCCESS);
            break;
        case 'v':
            vectorize = 1;
            break;
        case '?':
            fprintf(stderr, "%s: Unexpected option: %c\n", argv[0], optopt);
            exit(EXIT_FAILURE);
        case ':':
            fprintf(stderr, "%s: Missing value for: %c\n", argv[0], optopt);
            exit(EXIT_FAILURE);
        default:
            fprintf(stderr, USAGE, argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (optind != argc - 2)
    {
        fprintf(stderr, USAGE, argv[0]);
        exit(EXIT_FAILURE);
    }
    strncpy(oclfile, argv[optind], 255);
    oclfile[255] = '\0';
    impcode = atoi(argv[optind + 1]);
    if (impcode < 0 || impcode > 7)
    {
        fprintf(stderr, "%s: impcode value must be between 1 and 7\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    double *U = NULL, *x = NULL;
    float *U_f = NULL, *x_f = NULL;
    cl_status status;
    if (infile[0] != '\0')
    {
        printf("The matrix will be read from this file: %s\n", infile);
        U = read_matrix_n_from_file(infile, n);
    }
    else
    {
        printf("The matrix will be generated\n");
        U = generate_complete_matrix(seed, n);
    }

    switch (impcode)
    {
    case 1:
        x = Gaussian_elimination_no_pivot(U, NULL, n);
        break;
    case 2:
        strcat(oclfile, "/gaussian_elimination_no_pivot.ocl");
        clCreateStatus(&status, oclfile);
        x = Gaussian_elimination_no_pivot_gpu_lmem(U, NULL, n, &status);
        clFreeStatus(&status);
        break;
    case 3:
        U_f = d_to_f_array(U, n * n + n);
        strcat(oclfile, "/gaussian_elimination_no_pivot.ocl");
        clCreateStatus(&status, oclfile);
        if ((n + 1) & 3 || !vectorize)
            x_f = Gaussian_elimination_no_pivot_gpu_texture(U_f, NULL, n, &status);
        else
            x_f = Gaussian_elimination_no_pivot_gpu_texture_vec(U_f, NULL, n, &status);
        x = f_to_d_array(x_f, n);
        clFreeStatus(&status);
        break;
    case 4:
        strcat(oclfile, "/gaussian_elimination_no_pivot.ocl");
        clCreateStatus(&status, oclfile);
        if ((n + 1) & 3 || !vectorize)
            x = Gaussian_elimination_no_pivot_gpu_buffer(U, NULL, n, &status);
        else
            x = Gaussian_elimination_no_pivot_gpu_buffer_vec(U, NULL, n, &status);
        clFreeStatus(&status);
        break;
    case 5:
        x = Gaussian_elimination_pivot(U, NULL, n);
        break;
    case 6:
        U_f = d_to_f_array(U, n * n + n);
        strcat(oclfile, "/gaussian_elimination_pivot.ocl");
        clCreateStatus(&status, oclfile);
        if ((n + 1) & 3 || !vectorize)
            x_f = Gaussian_elimination_pivot_gpu_texture(U_f, NULL, n, &status);
        else
            x_f = Gaussian_elimination_pivot_gpu_texture_vec(U_f, NULL, n, &status);
        x = f_to_d_array(x_f, n);
        clFreeStatus(&status);
        break;
    case 7:
        strcat(oclfile, "/gaussian_elimination_pivot.ocl");
        clCreateStatus(&status, oclfile);
        if ((n + 1) & 3 || !vectorize)
            x = Gaussian_elimination_pivot_gpu_buffer(U, NULL, n, &status);
        else
            x = Gaussian_elimination_pivot_gpu_buffer_vec(U, NULL, n, &status);
        clFreeStatus(&status);
        break;
    default:
        break;
    }

    if (outfile[0] != '\0')
        freopen(outfile, "w", stdout);
    print_arr(x, n, 'x');

    free(U);
    free(x);
    if (U_f)
        free(U_f);
    if (x_f)
        free(x_f);

    return 0;
}
