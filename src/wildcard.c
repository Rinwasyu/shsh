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

#include <string.h>

#include "wildcard.h"

int wildcard_match(char *wc, char *str) {
	int is_match = 1;
	int wc_i = 0;
	int str_i = 0;

	while (wc_i < (int)strlen(wc) || str_i < (int)strlen(str)) {
		if (wc[wc_i] == '?') {
			// skip
		} else if (wc[wc_i] == '*') {
			if (wc_i == (int)strlen(wc) - 1) {
				wc_i = (int)strlen(wc);
				str_i = (int)strlen(str);
				break;
			}

			int is_match2 = 0;
			for (int i = str_i; i < (int)strlen(str); i++) {
				if (wildcard_match(&wc[wc_i + 1], &str[i])) {
					is_match2 = 1;
				}
			}
			is_match = is_match2;
			wc_i = (int)strlen(wc);
			str_i = (int)strlen(str);
			break;
		} else if (wc[wc_i] != str[str_i]) {
			is_match = 0;
		}
		wc_i++;
		str_i++;
	}

	if (wc_i != (int)strlen(wc) || str_i != (int)strlen(str)) {
		is_match = 0;
	}

	return is_match;
}
