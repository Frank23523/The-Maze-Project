CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
INCLUDES = -I./inc
SOURCES = $(wildcard src/*.c)
LIBS = -lSDL2 -lSDL2_image -lm
OUTPUT = stroll
MAP = maps/map.txt

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(OUTPUT) $(LIBS)

run: $(OUTPUT)
	./$(OUTPUT) $(MAP)

clean:
	rm -f $(OUTPUT)
