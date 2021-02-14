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

#define USAGE "Usage: %s [-i infname] [-o outfname] [-s seed] [-d dim] [-v] ocldname impcode\n"

/**
 * Shows the help tooltip
 * 
 * @param name Name of the executable
 */
void show_help_tooltip(const char *name);

#endif