/**
 * @file main.h
 * @author TendTo (https://github.com/TendTo)
 *
 * @brief Min file header
 * The execution of the application starts here
 */

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include "ocl_boiler.h"
#include "gaussian_elimination_pivot.h"
#include "gaussian_elimination_no_pivot.h"
#include "utility.h"

//Usage tooltip
#define USAGE \
"Usage: %s [-i infname] [-o outfname] [-s seed] [-d dim] [-v] ocldname impcode\n\
Use -h to know more informations\n"

//Help tooltip
#define HELP_TOOLTIP \
"---------------------------------------------------------------------\n\
Usage: %s [-i infname] [-o outfname] [-s seed] [-d dim] [-v] ocldname impcode\n\
---------------------------------------------------------------------\n\
impcode      -  implementation you want to use.\n\
                1: no pivot cpu\n\
                2: no pivot lmem [deprecated]\n\
                3: no pivot texture\n\
                4: no pivot buffer\n\
                5: partial pivot cpu\n\
                6: partial pivot texture\n\
                7: partial pivot buffer\n\
ocldname     -  path to the directory that contains the oclfiles\n\
\n\
-i infname   -  path of the file from which to fetch the matrix.\n\
                It must state all the elements of the complete matrix, separated by spaces, row by row\n\
-o outfname  -  path of the file where to print the result vector.\n\
                Defaults to use the standard output. If it is not specified, a dummy matrix will be generated\n\
-s seed      -  seed used to generate the matrix if -f is not specified.\n\
                Defaults to 123\n\
-d dim       -  number of unknowns of the matrix.\n\
                Defaults to 1000\n\
-v           -  enables vectorization, if the chosen implementation supports it.\n\
                The number of unknowns must be 1 less than a multiple of 4\n"

#endif