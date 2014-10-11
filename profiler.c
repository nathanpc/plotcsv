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
#include "gnuplot_i/gnuplot_i.h"

unsigned int read_csv_col(double **items, char *file, uint8_t col);
void setup_gnuplot(gnuplot_ctrl *gp);
void plot(gnuplot_ctrl *gp, char *title, double items[], unsigned int n);

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

	// Initialize gnuplot.
	gnuplot_ctrl *gp = gnuplot_init();
	setup_gnuplot(gp);

	double *heatsink = NULL;
	double *ambient = NULL;
	unsigned int n = read_csv_col(&heatsink, argv[1], 2);
	read_csv_col(&ambient, argv[1], 3);

	plot(gp, "Heatsink", heatsink, n);
	plot(gp, "Ambient", ambient, n);
	sleep(5);
	gnuplot_close(gp);

	return EXIT_SUCCESS;
}

/**
 * Counts the number of lines in a CSV file.
 *
 * @param file CSV filename.
 * @return Number of lines in the file.
 */
unsigned int csv_lines(char *file) {
	FILE *csv = fopen(file, "r");
	unsigned int n = 0;
	char c;

	if (csv == NULL) {
		printf("Couldn't open file: %s\n", file);
		exit(EXIT_FAILURE);
	}

	while (!feof(csv)) {
		c = fgetc(csv);
		if (c == '\n') {
			n++;
		}
	}

	return n;
}

/**
 * Parses a CSV file and fetches a specific column from each line.
 *
 * @param items Array of items.
 * @param file CSV filename.
 * @param col Column to fetch.
 * @return Number of items inside the array.
 */
unsigned int read_csv_col(double **items, char *file, uint8_t col) {
	FILE *csv = fopen(file, "r");
	char *line = NULL;
	char *sitem = NULL;
	size_t llen = 0;
	uint8_t citem = 0;
	unsigned int nitems = csv_lines(file);
	double *_items = malloc(sizeof(double) * nitems);

	if (csv == NULL) {
		printf("Couldn't open file: %s\n", file);
		exit(EXIT_FAILURE);
	}

	nitems = 0;
	while (getline(&line, &llen, csv) != -1) {
		sitem = strtok(line, ",");
		while (citem != col) {
			sitem = strtok(NULL, ",");
			citem++;
		}

		_items[nitems] = atof(sitem);
		citem = 0;
		nitems++;
	}

	fclose(csv);
	*items = _items;

	return nitems;
}

/**
 * Setup the gnuplot stuff.
 *
 * @param gp gnuplot_ctrl object to be setup.
 */
void setup_gnuplot(gnuplot_ctrl *gp) {
	// Disable the legend.
	//gnuplot_cmd(gp, "set key off");

	// Setup the axis.
	// TODO: Create a struct with axis name data.
	gnuplot_set_xlabel(gp, "Time (s)");
	gnuplot_set_ylabel(gp, "Temperature (C)");

	// Set plot style.
	gnuplot_setstyle(gp, "lines");
}

/**
 * Plots the data.
 *
 * @param title Plot title.
 * @param items Array of doubles.
 * @param n Number of items in the array.
 */
void plot(gnuplot_ctrl *gp, char *title, double items[], unsigned int n) {
	gnuplot_plot_x(gp, items, n, title);
}

