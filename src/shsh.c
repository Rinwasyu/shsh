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
#include "option.h"
#include "prompt.h"

/*
TODO: ↑ → ↓ ←
TODO: input
TODO: TAB
TODO: Awesome error messages
TODO: Split file (priority : VERY_HIGH) ...done
TODO: Run script file
TODO: REFACTOR
TODO: Use global variables
*/

void shsh_init() {
	prompt_init();
}

void shsh_exit() {
	prompt_exit();
}

int main(int argc, char **argv) {
	option_readOptions(argc, argv);

	shsh_init();
	prompt_loop();
	shsh_exit();

	// See you!
	printf("\n");
	return 0;
}
