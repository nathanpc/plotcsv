/**
 *  profiler.c
 *  A simple program to plot data from CSV files.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "gnuplot_i.h"

/**
 * Parses a CSV file and fetches a specific column from each line.
 *
 * @param file CSV filename.
 * @param col Column to fetch.
 */
void read_csv_col(const char *file, const uint8_t col) {
	FILE *csv = fopen(file, "r");
	char *line = NULL;
	char *item = NULL;
	size_t llen = 0;
	uint8_t citem = 0;

	if (csv == NULL) {
		printf("Couldn't open file: %s\n", file);
		exit(EXIT_FAILURE);
	}

	while (getline(&line, &llen, csv) != -1) {
		item = strtok(line, ",");
		while (citem != col) {
			item = strtok(NULL, ",");
			citem++;
		}

		printf("%s - ", item);
		citem = 0;
	}

	fclose(csv);
	if (line) {
		free(line);
	}
}

/**
 * No need to explain this.
 *
 * @param argc Number of arguments.
 * @param argv Arguments array.
 * @return Return code.
 */
int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: %s <file.csv>\n", argv[0]);
		return EXIT_FAILURE;
	}

	read_csv_col(argv[1], 2);

	return EXIT_SUCCESS;
}

/**
 * Plots the data.
 *
 * @param title Plot title.
 * @param items Array of doubles.
 * @param n Number of items in the array.
 */
void plot(char *title, double items[], unsigned int n) {
	// Initialize gnuplot.
	gnuplot_ctrl *gp;
	gp = gnuplot_init();

	// Disable the legend.
	gnuplot_cmd(gp, "set key off");

	// Setup the axis.
	// TODO: Create a struct with axis name data.
	gnuplot_set_xlabel(gp, "Time (s)");
	gnuplot_set_ylabel(gp, "Temperature (C)");

	// Plot.
	gnuplot_setstyle(gp, "lines");
	gnuplot_plot_x(gp, items, n, title);
	sleep(5);

	gnuplot_close(gp);
}

