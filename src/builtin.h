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

#ifndef SHSH_BUILTIN_H
#define SHSH_BUILTIN_H

void builtin_cd(char *arg);
void builtin_pwd(void);
void builtin_help(void);
int builtin_exit(void);

#endif
