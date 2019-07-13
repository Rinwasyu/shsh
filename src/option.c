/*
 * Copyright 2019 Rinwasyu
 *
 * This file is part of shsh.
 *
 * Shsh is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shsh is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shsh.h"
#include "command.h"
#include "option.h"

void option_readOptions(int argc, char **argv) {
	// Read options
	// TODO: Support other options
	// TODO: Support -abc...
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(&argv[i][1], "c") == 0) {
				// -c option
				if (argc > i+1) {
					command_exec(argv[i+1]);
				} else {
					printf("%s: -c: option requires an argument\n", PROG_NAME);
					exit(-1);
				}
				exit(0);
			} else if (strcmp(&argv[i][1], "v") == 0 || strcmp(&argv[i][1], "-version") == 0) {
				// -v or --version option
				option_version();
				exit(0);
			} else if (strcmp(&argv[i][1], "h") == 0 || strcmp(&argv[i][1], "-help") == 0) {
				// -h or --help option
				option_help();
				exit(0);
			} else {
				// Unknown option
				option_help();
				exit(2);
			}
		}
	}
}

void option_version() {
	printf("%s, version %s\n", PROG_FULLNAME, PROG_VERSION);
	printf("github.com/Rinwasyu/%s\n", PROG_NAME);
}

void option_help() {
	printf("Usage: %s [OPTION] ...\n", PROG_NAME);
	printf("Options:\n");
	printf("\t-c command\n");
	printf("\t-v, --version\n");
	printf("\t-h, --help\n");
}
