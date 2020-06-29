/*
 * Copyright 2019,2020 Rinwasyu
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
#include <signal.h>

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

	char *args[BUF_SIZE] = {0};
	int n_args = 0;
	signal(SIGINT, SIG_IGN);

	for (int i = 0; i < (int)strlen(shsh_command); i++) {
		if (shsh_command[i] == ' ') {
			continue;
		} else {
			char *arg = (char *)malloc(sizeof(char) * BUF_SIZE);
			memset(arg, 0, sizeof(char) * BUF_SIZE);
			for (; strlen(arg) < BUF_SIZE && i < (int)strlen(shsh_command) && shsh_command[i] != ' '; i++) {
				arg[strlen(arg)] = shsh_command[i];
			}
			// wildcard (maybe filenames_wildcard is not working correctly)
			char **file = filenames_wildcard(".", arg);
			for (int j = 0; file[j] != NULL; j++) {
				args[n_args] = file[j];
				n_args++;
			}
		}
	}

	if (n_args == 0)
		return;

	if (strlen(args[0]) <= 0)
		return;

	for (int i = 0; i < n_args; i++) {
		for (int j = 0; j < (int)strlen(args[i]); j++) {
			if (args[i][j] == '\'' || args[i][j] == '"' || args[i][j] == '`') {
				char qmark = args[i][j];
				char *new = (char *)malloc(sizeof(char) * BUF_SIZE);
				memset(new, 0, sizeof(char) * BUF_SIZE);
				snprintf(new, j+1, "%s", args[i]);
				for (j++; args[i][j] != qmark && j < (int)strlen(args[i]); j++)
					new[strlen(new)] = args[i][j];
				snprintf(&new[strlen(new)], BUF_SIZE - j, "%s", &args[i][j+1]);
				free(args[i]);
				args[i] = new;
			}
		}
	}

	// Built-in commands
	if (builtin_exec(args) == 0)
		return;

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
			snprintf(prog_path + strlen(prog_path), BUF_SIZE - strlen(prog_path), "%s", args[0]);
			// PATH/prog
			execv(prog_path, args);
		} while ((path = strtok(NULL, ":")) != NULL);
		// ./prog
		execv(args[0], args);
		printf("%s: %s: command not found\n", PROG_NAME, args[0]);

		exit(-1);
	}
	waitpid(pid, NULL, 0);
}
