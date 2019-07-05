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
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

#define PROG_NAME "shsh"
#define PROG_FULLNAME "(((ง'ω')و三 ง'ω')ڡ≡ shsh"
#define PROG_VERSION "0.0.x-alpha"

/*
TODO: ↑ → ↓ ←
TODO: input
*/

void print_version() {
	printf("%s, version %s\n", PROG_FULLNAME, PROG_VERSION);
	printf("github.com/Rinwasyu/%s\n", PROG_NAME);
}

void print_help() {
	printf("Usage: %s [OPTION] ...\n", PROG_NAME);
	printf("Options:\n");
	printf("\t-c command\n");
	printf("\t-v, --version\n");
	printf("\t-h, --help\n");
}

void exec_command(char *command) {
	// TODO: Search for PATH
	// TODO: Support built-in commands (cd, export, etc.)
	// TODO: Run ShellScript
	char *prog = NULL;
	char *args[BUF_SIZE] = {NULL};

	if (strlen(command) <= 0) return;

	prog = strtok(command, " ");

	args[0] = prog;
	for (int i = 1; i < BUF_SIZE && (args[i] = strtok(NULL, " ")) != NULL; i++);

	char *shsh_path = getenv("PATH");
	char *path = strtok(shsh_path, ":");
	do {
		char prog_path[BUF_SIZE] = {0};
		snprintf(prog_path, BUF_SIZE, path);
		snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), "/");
		snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), prog);
		// PATH/prog
		execv(prog_path, args);
	} while ((path = strtok(NULL, ":")) != NULL);
	// ./prog
	execv(prog, args);

	printf("%s: %s: command not found\n", PROG_NAME, command);
}

int main(int argc, char **argv, char **envp) {
	// Read options
	// TODO: Support other options
	// TODO: Support -abc...
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(&argv[i][1], "c") == 0) {
				// -c option
				int pid = fork();
				if (pid == 0) {
					if (argc > i+1) {
						exec_command(argv[i+1]);
					} else {
						printf("%s: -c: option requires an argument\n", PROG_NAME);
					}
					return -1;
				}
				waitpid(pid, NULL, 0);
				return 0;
			} else if (strcmp(&argv[i][1], "v") == 0 || strcmp(&argv[i][1], "-version") == 0) {
				// -v or --version option
				print_version();
				return 0;
			} else if (strcmp(&argv[i][1], "h") == 0 || strcmp(&argv[i][1], "-help") == 0) {
				// -h or --help option
				print_help();
				return 0;
			} else if (strcmp(&argv[i][1], "envp") == 0) {
				// for DEBUG
				printf("envp:\n");
				for (int i = 0; envp[i] != NULL; i++) {
					printf("%s\n", envp[i]);
				}
				return 0;
			} else {
				// Unknown option
				print_help();
				return 2;
			}
		}
	}

	char c;
	char *command = (char *)malloc(sizeof(char) * BUF_SIZE);
	char **command_history = (char **)malloc(sizeof(char *) * BUF_SIZE);

	printf("<%s@%s> $ ", getenv("LOGNAME"), strtok(getenv("HOSTNAME"), "."));
	while ((c = getchar()) != EOF) {
		if (c == 10) { // Enter
			int pid = fork();
			if (pid == 0) {
				exec_command(command);
				return -1;
			}
			waitpid(pid, NULL, 0);
			memset(command, 0, sizeof(char) * BUF_SIZE);
			printf("<%s@%s> $ ", getenv("LOGNAME"), strtok(getenv("HOSTNAME"), "."));
		} else {
			if (strlen(command) < BUF_SIZE) {
				command[strlen(command)] = c;
			}
		}
	}

	// See you!
	printf("\n");
	return 0;
}
