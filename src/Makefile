.PHONY: all clean

CC           := gcc
CFLAGS       := -Wall -Wextra -O3

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
