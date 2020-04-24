CC = cc
CFLAGS = -std=c11 -Wall -g $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2)
SOURCES = opcodes.c chip8.c main.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = chip8

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(EXECUTABLE)
