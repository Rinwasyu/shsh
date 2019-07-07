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
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 1024

#define PROG_NAME "shsh"
#define PROG_FULLNAME "(((ง'ω')و三 ง'ω')ڡ≡ shsh"
#define PROG_VERSION "0.0.2.5-alpha"

/*
TODO: ↑ → ↓ ←
TODO: input
TODO: TAB
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

void builtin_cd(char *arg) {
	//TODO: WILDCARD
	char shsh_pwd[BUF_SIZE] = {0};
	snprintf(shsh_pwd, BUF_SIZE, getenv("PWD"));

	if (strcmp(arg, "..") == 0) {
		for (int i = strlen(shsh_pwd) - 1; i > 1; i--) {
			if (shsh_pwd[i] == '/') {
				shsh_pwd[i] = '\0';
				break;
			}
		}
	} else {
		if (shsh_pwd[strlen(shsh_pwd)-1] != '/') {
			snprintf(shsh_pwd + strlen(shsh_pwd), BUF_SIZE - strlen(shsh_pwd), "/");
		}
		snprintf(shsh_pwd + strlen(shsh_pwd), BUF_SIZE - strlen(shsh_pwd), arg);
	}

	if (chdir(shsh_pwd) == 0) { // Success
		setenv("PWD", shsh_pwd, 1);
	} else {
		printf("Error\n");
	}
}

void exec_command(char *command) {
	// TODO: Search for PATH...done
	// TODO: Support built-in coumands (cd...done, pwd...done, export, etc.)
	// TODO: Run ShellScript
	// TODO: Run program correctly if your own program name conflict with the system program
	char *prog = NULL;
	char *args[BUF_SIZE] = {NULL};

	if (strlen(command) <= 0) return;

	prog = strtok(command, " ");

	args[0] = prog;
	for (int i = 1; i < BUF_SIZE && (args[i] = strtok(NULL, " ")) != NULL; i++);

	// Built-in commands
	if (strcmp(prog, "cd") == 0) {
		// cd command
		builtin_cd(args[1]);
		return;
	} else if (strcmp(prog, "pwd") == 0) {
		printf("%s\n", getenv("PWD"));
		return;
	}

	int pid = fork();
	if (pid == 0) {
		shsh_exit();
		signal(SIGINT, SIG_DFL);

		char shsh_path[BUF_SIZE] = {0};
		snprintf(shsh_path, BUF_SIZE, getenv("PATH"));
		char *path = strtok(shsh_path, ":");
		do {
			char prog_path[BUF_SIZE] = {0};
			snprintf(prog_path, BUF_SIZE, path);
			if (prog_path[strlen(prog_path)-1] != '/') { // /hogehoge →  /hogehoge/
				snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), "/");
			}
			snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), prog);
			// PATH/prog
			execv(prog_path, args);
		} while ((path = strtok(NULL, ":")) != NULL);
		// ./prog
		execv(prog, args);
		printf("%s: %s: command not found\n", PROG_NAME, command);

		exit(-1);
	}
	waitpid(pid, NULL, 0);
	shsh_init();
}

int main(int argc, char **argv, char **envp) {
	// Read options
	// TODO: Support other options
	// TODO: Support -abc...
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (strcmp(&argv[i][1], "c") == 0) {
				// -c option
				if (argc > i+1) {
					exec_command(argv[i+1]);
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

	char c;
	char *command = (char *)malloc(sizeof(char) * BUF_SIZE);
	char **command_history = (char **)malloc(sizeof(char *) * BUF_SIZE);
	int cursor_x = 0;
	// Input mode
	enum {Insert, Esc, Bracket, Numpad} mode = Insert;

	char shsh_logname[BUF_SIZE] = {0};
	char shsh_hostname[BUF_SIZE] = {0};

	snprintf(shsh_logname, BUF_SIZE, getenv("LOGNAME"));
	snprintf(shsh_hostname, BUF_SIZE, getenv("HOSTNAME"));

	signal(SIGINT, SIG_IGN);

	shsh_init();
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

				exec_command(command);

				memset(command, 0, sizeof(char) * BUF_SIZE);
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
			}
			// TODO:
		} else if (mode == Numpad) {
			mode = Insert;
		}
	}


	// See you!
	printf("\n");
	return 0;
}
