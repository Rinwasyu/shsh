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

#include "shsh.h"
#include "builtin.h"
#include "command.h"
#include "filenames.h"

void command_exec(char *shsh_command) {
	// TODO: Support built-in coumands (cd...done, pwd...done, help...done, export, etc.)
	// TODO: Run ShellScript
	// TODO: Run program correctly if your own program name conflict with the system program
	// TODO: WILDCARD
	// TODO: Job control
	// TODO: Support "" and '' in args
	// TODO: Support pipe
	// TODO: Support redirect
	char command[BUF_SIZE] = {0};
	char prog[BUF_SIZE] = {0};
	char *args[BUF_SIZE] = {NULL};

	signal(SIGINT, SIG_IGN);

	snprintf(command, BUF_SIZE, "%s", shsh_command);

	if (strlen(command) <= 0) return;

	int command_i = 0;
	for (; command_i < (int)strlen(command) && command[command_i] == ' '; command_i++); // Skip
	for (int i = 0; command_i < (int)strlen(command) && command[command_i] != ' '; i++, command_i++) {
		prog[i] = command[command_i];
	}

	args[0] = prog;

	// args
	for (int i = 1; i < BUF_SIZE && command_i < (int)strlen(command); i++, command_i++) {
		char arg[BUF_SIZE] = {0};
		for (; command[command_i] == ' ' && command_i < (int)strlen(command); command_i++); // Skip
		if (command[command_i] == '"' || command[command_i] == '\'' || command[command_i] == '`') {
			char q_mark = command[command_i];
			for (command_i++; command[command_i] != q_mark && command_i < (int)strlen(command); command_i++) {
				if (command[command_i] == '\\') {
					if (command_i+1 < (int)strlen(command)) {
						if (command[command_i+1] == '\'' || command[command_i+1] == '"')
							command_i++;
					} else {
						printf("Error\n");
					}
				}
				arg[strlen(arg)] = command[command_i];
			}
			args[i] = (char *)malloc(sizeof(char) * BUF_SIZE);
			memset(args[i], 0, sizeof(char) * BUF_SIZE);
			snprintf(args[i], BUF_SIZE, "%s", arg);
		} else {
			for (; command[command_i] != ' ' && command_i < (int)strlen(command); command_i++) {
				arg[strlen(arg)] = command[command_i];
			}
			char **filenames = filenames_wildcard(".", arg);
			for (int j = 0; filenames[j] != NULL && i < BUF_SIZE; j++) {
				args[i] = (char *)malloc(sizeof(char) * BUF_SIZE);
				memset(args[i], 0, sizeof(char) * BUF_SIZE);
				snprintf(args[i], BUF_SIZE, "%s", filenames[j]);
				if (filenames[j+1] != NULL) i++;
			}
		}
	}

	// Built-in commands
	if (strcmp(prog, "cd") == 0) {
		if (args[1] != NULL) {
			builtin_cd(args[1]);
		} else {
			builtin_cd("~");
		}
		return;
	} else if (strcmp(prog, "pwd") == 0) {
		builtin_pwd();
		return;
	} else if (strcmp(prog, "help") == 0) {
		builtin_help();
		return;
	} else if (strcmp(prog, "exit") == 0) {
		builtin_exit();
		return;
	}

	int pid = fork();
	if (pid == 0) {
		char shsh_path[BUF_SIZE] = {0};
		snprintf(shsh_path, BUF_SIZE, "%s", getenv("PATH"));
		char *path = strtok(shsh_path, ":");

		signal(SIGINT, SIG_DFL);

		do {
			char prog_path[BUF_SIZE] = {0};
			snprintf(prog_path, BUF_SIZE, "%s", path);
			if (prog_path[strlen(prog_path)-1] != '/') { // /hogehoge →  /hogehoge/
				snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), "/");
			}
			snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), "%s", prog);
			// PATH/prog
			execv(prog_path, args);
		} while ((path = strtok(NULL, ":")) != NULL);
		// ./prog
		execv(prog, args);
		printf("%s: %s: command not found\n", PROG_NAME, command);

		exit(-1);
	}
	waitpid(pid, NULL, 0);
}
