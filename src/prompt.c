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
 
 #include "shsh.h"
 #include "command.h"
 #include "prompt.h"

void prompt_init() {
	system("stty stop undef");
	system("stty start undef");
	system("stty -echo -icanon min 1 time 0");
	signal(SIGINT, SIG_IGN);
}

void prompt_exit() {
	system("stty sane");
	signal(SIGINT, SIG_DFL);
}

void prompt_print() {
	// TODO: print current directory name
	char shsh_logname[BUF_SIZE] = {0};
	char shsh_hostname[BUF_SIZE] = {0};
	char shsh_pwd[BUF_SIZE] = {0};

	snprintf(shsh_logname, BUF_SIZE, getenv("LOGNAME"));
	snprintf(shsh_hostname, BUF_SIZE, getenv("HOSTNAME"));
	snprintf(shsh_pwd, BUF_SIZE, getenv("PWD"));
	
	printf("<%s@%s> %s ", shsh_logname, strtok(shsh_hostname, "."), strcmp(shsh_logname, "root") == 0 ? "#":"$");
}

void prompt_loop() {
	// TODO: Support command history
	char c;
	char *command = (char *)malloc(sizeof(char) * BUF_SIZE);
	//char **command_history = (char **)malloc(sizeof(char *) * BUF_SIZE);
	int cursor_x = 0;
	// Input mode
	enum {Insert, Esc, Bracket, Numpad} mode = Insert;
	
	prompt_print();
	while (1) {
		// DEBUG
		//printf("keycode : %d\n", c);
		// TODO: here
		if ((c = getchar()) == EOF) {
			shsh_exit();
			break;
		}
		
		if (mode == Insert) {
			if (c == 4) { // Ctrl-D
				printf("exit");
				break;
			} else if (c == 9) { // Tab
			} else if (c == 10) { // Enter
				printf("\n");

				prompt_exit();
				command_exec(command);
				prompt_init();

				memset(command, 0, sizeof(char) * BUF_SIZE);
				
				prompt_print();
				
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
			} else if (c == 126) {
				mode = Insert;
			}
		} else if (mode == Numpad) {
			mode = Insert;
		}
	}
}
