
CC = g++
CCFLAGS = -Wall -std=c++17
SOURCES = $(wildcard ./src/*.cc)
OBJECTS = $(SOURCES:./src/%.cc=./obj/%.o)

HEADER = ./include/

.PHONY: clean

dragonshell: $(OBJECTS)
	$(CC) $(CCFLAGS) $< -o $@

$(OBJECTS): $(SOURCES)
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) $< -c -o $@

clean:
	@rm -rf dragonshell
	@rm -rf $(OBJECTS)

