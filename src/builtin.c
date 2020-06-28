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

#include "shsh.h"
#include "wildcard.h"
#include "filenames.h"
#include "builtin.h"

int builtin_exec(char **args) {
	if (strcmp(args[0], "cd") == 0) {
		if (args[1] != NULL) {
			builtin_command_cd(args[1]);
		} else {
			builtin_command_cd("~");
		}
		return 0;
	} else if (strcmp(args[0], "pwd") == 0) {
		builtin_command_pwd();
		return 0;
	} else if (strcmp(args[0], "help") == 0) {
		builtin_command_help();
		return 0;
	} else if (strcmp(args[0], "exit") == 0) {
		builtin_command_exit();
		return 0;
	} else {
		return 1;
	}
}

void builtin_command_cd(char *arg) {
	// TODO: WILDCARD
	// TODO: Add more features
	char shsh_pwd[BUF_SIZE] = {0};

	if (arg[0] != '/') {
		snprintf(shsh_pwd, BUF_SIZE, "%s", getenv("PWD"));
	}

	if (arg[0] == '/') {
		snprintf(shsh_pwd + strlen(shsh_pwd), BUF_SIZE - strlen(shsh_pwd), "%s", arg);
	} else if (strcmp(arg, "~") == 0) {
		memset(shsh_pwd, 0, sizeof(char) * BUF_SIZE);
		snprintf(shsh_pwd, BUF_SIZE, "%s", getenv("HOME"));
	} else if (strcmp(arg, "..") == 0) {
		for (int i = (int)strlen(shsh_pwd) - 1; i > 0; i--) {
			if (shsh_pwd[i] != '/') {
				shsh_pwd[i] = '\0';
			} else {
				if (strlen(shsh_pwd) > 1)
					shsh_pwd[i] = '\0';
				break;
			}
		}
	} else {
		char dir_name[BUF_SIZE] = {0};
		snprintf(dir_name, BUF_SIZE, "%s", filenames_wildcard(".", arg)[0]);

		if (shsh_pwd[strlen(shsh_pwd)-1] != '/') {
			snprintf(shsh_pwd + strlen(shsh_pwd), BUF_SIZE - strlen(shsh_pwd), "/");
		}
		snprintf(shsh_pwd + strlen(shsh_pwd), BUF_SIZE - strlen(shsh_pwd), "%s", dir_name);
	}

	if (chdir(shsh_pwd) == 0) { // Success
		setenv("PWD", shsh_pwd, 1);
	} else {
		printf("Error\n");
	}
}

void builtin_command_pwd() {
	printf("%s\n", getenv("PWD"));
}

void builtin_command_help() {
	printf("builtin commands :\n");
	printf("cd, pwd, help(this)\n");
}

int builtin_command_exit() {
	exit(0);
}
