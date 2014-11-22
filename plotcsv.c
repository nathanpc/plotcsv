/**
 *  plotcsv.c
 *  A simple program to plot data from CSV files.
 *
 *  @author Nathan Campos <nathanpc@dreamintech.net>
 */

#include "plotcsv.h"

/**
 * No need to explain this.
 *
 * @param argc Number of arguments.
 * @param argv Arguments array.
 * @return Return code.
 */
int main(int argc, char **argv) {
	bool running = true;
	char *script_file = NULL;
	char *csv_file = NULL;
	char *ctok = NULL;
	gnuplot_ctrl *gp = gnuplot_init();
	char prompt[64] = "> ";

	if (argc == 2) {
		if (argv[1][strlen(argv[1]) - 3] == '.' &&
				argv[1][strlen(argv[1]) - 2] == 'p' &&
				argv[1][strlen(argv[1]) - 1] == 'c') {
			// Script file.
			script_file = argv[1];
			running = parse_script(script_file, ctok, &csv_file, gp, prompt);
		} else {
			// CSV file.
			csv_file = argv[1];
			generate_prompt(prompt, csv_file);
		}
	}

	while (running) {
		char *buffer = NULL;

		buffer = readline(prompt);
		if (buffer && *buffer) {
			add_history(buffer);
			ctok = strtok(buffer, " ");
		} else {
			continue;
		}

		running = parse_cmd_line(ctok, &csv_file, gp, prompt, false);
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
bool parse_cmd_line(char *ctok, char **csv_file, gnuplot_ctrl *gp, char *prompt, bool quiet) {
	// Command.
	if (!strcmp(ctok, "quit") || !strcmp(ctok, "exit")) {
		return false;
	} else if (!strcmp(ctok, "legend")) {
		// Toogle legend.
		if (!strcmp(strtok(NULL, " "), "off")) {
			gnuplot_cmd(gp, "set key off");
			if (!quiet) {
				printf("Legend turned off.\n");
			}
		} else {
			gnuplot_cmd(gp, "set key on");
			if (!quiet) {
				printf("Legend turned on.\n");
			}
		}
	} else if (!strcmp(ctok, "xlabel") || !strcmp(ctok, "ylabel")) {
		// Set xy label.
		char arg[64] = "";
		char xy = ctok[0];

		parse_spaced_arg(arg, ctok);
		if (!quiet) {
			printf("%clabel set to \"%s\"\n", xy, arg);
		}

		switch (xy) {
			case 'x':
				gnuplot_set_xlabel(gp, arg);
				break;
			case 'y':
				gnuplot_set_ylabel(gp, arg);
		}
	} else if (!strcmp(ctok, "plot")) {
		// Plot data.
		char lg_title[64] = "";
		uint8_t col = atoi(strtok(NULL, " "));
		double *items = NULL;
		unsigned int n = 0;

		parse_spaced_arg(lg_title, ctok);
		n = read_csv_col(&items, *csv_file, col);
		gnuplot_setstyle(gp, "lines");
		gnuplot_plot_x(gp, items, n, lg_title);
	} else if (!strcmp(ctok, "gp")) {
		// Execute raw gnuplot command.
		char gp_cmd[1024] = "";

		parse_spaced_arg(gp_cmd, ctok);
		gnuplot_cmd(gp, gp_cmd);
	} else if (!strcmp(ctok, "load")) {
		// Load CSV.
		char filename[2048] = "";
		parse_spaced_arg(filename, ctok);

		*csv_file = malloc(strlen(filename) + 1);
		strcpy(*csv_file, filename);
		generate_prompt(prompt, *csv_file);
	} else {
		printf("Invalid command: %s\n", ctok);
	}

	return true;
}

/**
 * Parses a script file.
 *
 * @param script_file Script file location.
 * @param ctok Command string in strtok.
 * @param csv_file CSV file location.
 * @param gp gnuplot object.
 * @return True if the program should continue running.
 */
bool parse_script(char *script_file, char *ctok, char **csv_file, gnuplot_ctrl *gp, char *prompt) {
	FILE *script = fopen(script_file, "r");
	char *line = NULL;
	size_t llen = 0;
	bool cont = true;

	if (script == NULL) {
		printf("Couldn't open script: %s\n", script_file);
		exit(EXIT_FAILURE);
	}

	printf("Reading %s...\n", script_file);
	while (getline(&line, &llen, script) != -1 && cont) {
		line[strlen(line) - 1] = '\0';

		// Comment or empty line.
		if (line[0] == '#' || strlen(line) == 0) {
			continue;
		}

		add_history(line);
		ctok = strtok(line, " ");
		cont = parse_cmd_line(ctok, csv_file, gp, prompt, true);
	}

	fclose(script);
	return cont;
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
 * Parses a spaced argument.
 *
 * @param arg Where the argument will be stored.
 * @param ctok strtok string.
 */
void parse_spaced_arg(char *arg, char *ctok) {
	ctok = strtok(NULL, " ");

	while (ctok != NULL) {
		strcat(arg, ctok);
		strcat(arg, " ");

		ctok = strtok(NULL, " ");
	}

	arg[strlen(arg) - 1] = '\0';  // Remove the last space.
}

/**
 * Generate a prompt string.
 *
 * @param prompt Prompt string.
 * @param csv_file CSV filename.
 */
void generate_prompt(char *prompt, char *csv_file) {
	char *filename = malloc(strlen(csv_file));
	strcpy(filename, csv_file);
	char *ctok = strtok(filename, "/");

	while (ctok != NULL) {
		strncpy(prompt, ctok, 60);
		ctok = strtok(NULL, "/");
	}

	strcat(prompt, "> ");
	ctok = NULL;
}

