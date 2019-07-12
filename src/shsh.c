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
#include <dirent.h>
#include <signal.h>

#include "shsh.h"
#include "builtin.h"
#include "command.h"

/*
TODO: ↑ → ↓ ←
TODO: input
TODO: TAB
TODO: Awesome error messages
TODO: Split file (priority : VERY_HIGH)
TODO: Run script file
TODO: REFACTOR
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

void shsh_init() {
	system("stty stop undef");
	system("stty start undef");
	system("stty -echo -icanon min 1 time 0");
}

void shsh_exit() {
	system("stty sane");
}

// TODO: Make main function shorter (Split function)
int main(int argc, char **argv, char **envp) {
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
					return -1;
				}
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

	// Prompt
	// TODO: Support command history
	char c;
	char *command = (char *)malloc(sizeof(char) * BUF_SIZE);
	//char **command_history = (char **)malloc(sizeof(char *) * BUF_SIZE);
	int cursor_x = 0;
	// Input mode
	enum {Insert, Esc, Bracket, Numpad} mode = Insert;

	char shsh_logname[BUF_SIZE] = {0};
	char shsh_hostname[BUF_SIZE] = {0};
	char shsh_pwd[BUF_SIZE] = {0};

	snprintf(shsh_logname, BUF_SIZE, getenv("LOGNAME"));
	snprintf(shsh_hostname, BUF_SIZE, getenv("HOSTNAME"));
	snprintf(shsh_pwd, BUF_SIZE, getenv("PWD"));

	signal(SIGINT, SIG_IGN);

	shsh_init();
	// TODO: print current directory name
	printf("<%s@%s> $ ", shsh_logname, strtok(shsh_hostname, "."));
	while ((c = getchar()) != EOF) {
		// DEBUG
		//printf("keycode : %d\n", c);
		// TODO: here
		if (mode == Insert) {
			if (c == 4) { // Ctrl-D
				printf("exit");
				shsh_exit();
				break;
			} else if (c == 9) { // Tab
			} else if (c == 10) { // Enter
				printf("\n");

				command_exec(command);

				memset(command, 0, sizeof(char) * BUF_SIZE);
				memset(shsh_pwd, 0, sizeof(char) * BUF_SIZE);

				snprintf(shsh_pwd, BUF_SIZE, getenv("PWD"));

				printf("<%s@%s> $ ", shsh_logname, strtok(shsh_hostname, "."));
				mode = Insert;
				cursor_x = 0;
			} else if (c == 27) {
				mode = Esc;
			} else if (c == 127) {
				// Backspace
				if (strlen(command) > 0 && cursor_x > 0) {
					int s_len = strlen(command);
					cursor_x--;
					printf("\b");
					for (int i = cursor_x; i < s_len; i++) {
						command[i] = command[i + 1];
						printf("%c", command[i] ? command[i] : ' ');
					}
					for (int i = cursor_x; i < s_len; i++)
						printf("\b");
				}
			} else {
				// input
				if (strlen(command) < BUF_SIZE - 1) {
					for (int i = cursor_x; i < strlen(command); i++)
						printf(" ");
					for (int i = cursor_x; i < strlen(command); i++)
						printf("\b");

					for (int i = cursor_x, ch = c; i < strlen(command) + 1; i++) {
						char swap = command[i];
						printf("%c", ch);
						command[i] = ch;
						ch = swap;
					}

					for (int i = 0; i < strlen(command) - cursor_x - 1; i++)
						printf("\b");
					cursor_x++;
				}
			}
		} else if (mode == Esc) {
			if (c == 79 || c == 91) {
				mode = Bracket;
			} else {
				mode = Insert;
			}
		} else if (mode == Bracket) {
			if (c == 49 || c == 52) {
				mode = Numpad;
			} else if (c == 65) {
				mode = Insert;
			} else if (c == 66) {
				mode = Insert;
			} else if (c == 67) { // RIGHT→
				if (cursor_x < strlen(command)) {
					printf("\e[1C");
					cursor_x++;
				}
				mode = Insert;
			} else if (c == 68) { // Left←
				if (cursor_x > 0) {
					printf("\e[1D");
					cursor_x--;
				}
				mode = Insert;
			} else if (c == 70) { // End→→
				for (; command[cursor_x] != '\0'; cursor_x++) {
					printf("\e[1C");
				}
				mode = Insert;
			} else if (c == 72) { // Home←←
				for (; cursor_x > 0; cursor_x--) {
					printf("\e[1D");
				}
				mode = Insert;
			}
		} else if (mode == Numpad) {
			mode = Insert;
		}
	}


	// See you!
	printf("\n");
	return 0;
}
