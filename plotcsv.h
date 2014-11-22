/**
 *  plotcsv.h
 *  A simple program to plot data from CSV files.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#ifndef _PLOTCSV_H
#define _PLOTCSV_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "gnuplot_i/gnuplot_i.h"

void generate_prompt(char *prompt, char *csv_file);
void parse_spaced_arg(char *arg, char *ctok);
unsigned int read_csv_col(double **items, char *file, uint8_t col);
bool parse_cmd_line(char *ctok, char **csv_file, gnuplot_ctrl *gp, char *prompt, bool quiet);
bool parse_script(char *script_file, char *ctok, char **csv_file, gnuplot_ctrl *gp, char *prompt);


#endif
