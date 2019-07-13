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
	// TODO: Search for PATH...done
	// TODO: Support built-in coumands (cd...done, pwd...done, export, etc.)
	// TODO: Run ShellScript
	// TODO: Run program correctly if your own program name conflict with the system program
	// TODO: WILDCARD
	// TODO: Job control
	char command[BUF_SIZE] = {0};
	char *prog = NULL;
	char *args[BUF_SIZE] = {NULL};

	snprintf(command, BUF_SIZE, shsh_command);

	if (strlen(command) <= 0) return;

	prog = strtok(command, " ");

	args[0] = prog;


	// Built-in commands
	if (strcmp(prog, "cd") == 0) {
		// cd command
		char *arg;
		if ((arg = strtok(NULL, " ")) != NULL) {
			builtin_cd(arg);
		} else {
			builtin_cd("~");
		}
		return;
	} else if (strcmp(prog, "pwd") == 0) {
		builtin_pwd();
		return;
	}

	for (int i = 1; i < BUF_SIZE && (args[i] = strtok(NULL, " ")) != NULL; i++) {
		if (args[i][0] != '-' && args[i][0] != '"' && args[i][0] != '\'') {
			char **filenames = filenames_wildcard(".", args[i]);
			for (int j = 0; filenames[j] != NULL && i < BUF_SIZE; j++) {
				args[i] = filenames[j];
				if (filenames[j+1] != NULL) i++;
			}
		}
	}

	int pid = fork();
	if (pid == 0) {
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
}
