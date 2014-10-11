/**
 *  profiler.c
 *  A simple program to plot data from CSV files.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "gnuplot_i/gnuplot_i.h"

unsigned int read_csv_col(double **items, char *file, uint8_t col);
bool parse_cmd_line(char *ctok, char *csv_file, gnuplot_ctrl *gp);

/**
 * No need to explain this.
 *
 * @param argc Number of arguments.
 * @param argv Arguments array.
 * @return Return code.
 */
int main(int argc, char **argv) {
	bool running = true;
	char *csv_file;
	gnuplot_ctrl *gp = gnuplot_init();

	if (argc == 2) {
		csv_file = argv[1];
	}

	while (running) {
		char *buffer = NULL;
		char *ctok = NULL;

		buffer = readline("profiler> ");
		if (buffer && *buffer) {
			add_history(buffer);
			ctok = strtok(buffer, " ");
		} else {
			continue;
		}

		running = parse_cmd_line(ctok, csv_file, gp);
	}

	gnuplot_close(gp);
	return EXIT_SUCCESS;
}

/**
 * Parses a command.
 *
 * @param ctok Command string in strtok.
 * @param csv_file CSV file location.
 * @param gp gnuplot object.
 * @return True if the program should continue running.
 */
bool parse_cmd_line(char *ctok, char *csv_file, gnuplot_ctrl *gp) {
	if (!strcmp(ctok, "quit") || !strcmp(ctok, "exit")) {
		return false;
	} else if (!strcmp(ctok, "legend")) {
		if (!strcmp(strtok(NULL, " "), "off")) {
			gnuplot_cmd(gp, "set key off");
			printf("Legend turned off.\n");
		} else {
			gnuplot_cmd(gp, "set key on");
			printf("Legend turned on.\n");
		}
	} else if (!strcmp(ctok, "xlabel") || !strcmp(ctok, "ylabel")) {
		char arg[64] = "";
		char xy = ctok[0];
		ctok = strtok(NULL, " ");

		while (ctok != NULL) {
			strcat(arg, ctok);
			strcat(arg, " ");

			ctok = strtok(NULL, " ");
		}

		arg[strlen(arg) - 1] = '\0';  // Remove the last space.
		printf("%clabel set to \"%s\"\n", xy, arg);

		switch (xy) {
			case 'x':
				gnuplot_set_xlabel(gp, arg);
				break;
			case 'y':
				gnuplot_set_ylabel(gp, arg);
		}
	} else if (!strcmp(ctok, "plot")) {
		char lg_title[64] = "";
		uint8_t col = atoi(strtok(NULL, " "));
		double *items = NULL;
		unsigned int n = 0;

		ctok = strtok(NULL, " ");
		while (ctok != NULL) {
			strcat(lg_title, ctok);
			strcat(lg_title, " ");

			ctok = strtok(NULL, " ");
		}

		lg_title[strlen(lg_title) - 1] = '\0';  // Remove the last space.
		n = read_csv_col(&items, csv_file, col);
		gnuplot_setstyle(gp, "lines");
		gnuplot_plot_x(gp, items, n, lg_title);
	} else {
		printf("Invalid command: %s\n", ctok);
	}

	return true;
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

