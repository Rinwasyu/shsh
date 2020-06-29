#
# Copyright 2019,2020 Rinwasyu
# 
# This file is part of kot.
# 
# Kot is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Kot is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
# 

.PHONY: all clean

CC           := gcc
CFLAGS       := -std=c11 -Wall -Wextra -O3

PROGNAME     := shsh
EXECUTABLE   := $(PROGNAME)
SOURCES      := $(wildcard *.c)
OBJECTS      := $(SOURCES:%.c=%.o)
DEPENDENCIES := $(SOURCES:%.c=%.d)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -MD -c $<

clean:
	rm -f $(EXECUTABLE) $(OBJECTS) $(DEPENDENCIES)

-include $(DEPENDENCIES)
