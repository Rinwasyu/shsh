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
 
 #include "shsh.h"
 #include "wildcard.h"

char **filenames_wildcard(char *dirname, char *wildcard) {
	char **filenames = (char **)malloc(sizeof(char *) * BUF_SIZE);;
	int filenames_i = 0;

	DIR *dir;
	struct dirent *dp;

	if ((dir = opendir(dirname)) == NULL) {
		exit(-1);
	}

	for (dp = readdir(dir); dp != NULL; dp = readdir(dir)) {
		if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) { // exclude . and ..
			if (wildcard_match(wildcard, dp->d_name)) {
				filenames[filenames_i] = (char *)malloc(sizeof(char) * BUF_SIZE);
				memset(filenames[filenames_i], 0, sizeof(char) * BUF_SIZE);
				snprintf(filenames[filenames_i], BUF_SIZE, dp->d_name);
				filenames_i++;
			}
		}
	}

	if (filenames_i == 0) {
		filenames[0] = wildcard;
	}

	return filenames;
}
