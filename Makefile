
CC = g++
CCFLAGS = -Wall -std=c++17
SOURCES = $(wildcard ./src/*.cc)
OBJECTS = $(SOURCES:./src/%.cc=./obj/%.o)

.PHONY: clean compile

dragonshell: compile
	$(CC) $(CCFLAGS) $(OBJECTS) -o $@

compile: $(OBJECTS)

$(OBJECTS): $(SOURCES)
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) $< -c -o $@

clean:
	@rm -rf dragonshell
	@rm -rf $(OBJECTS)

compress:
	tar -caf dragonshell.tar.gz $(SOURCES)