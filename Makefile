
CC = g++
CCFLAGS = -Wall -std=c++17
SOURCES = $(wildcard ./src/*.cc)
OBJECTS = $(SOURCES:./src/%.cc=./obj/%.o)
BINNAME = lianshell

.PHONY: clean compile

$(BINNAME): compile
	$(CC) $(CCFLAGS) $(OBJECTS) -o $@

compile: $(OBJECTS)

$(OBJECTS): $(SOURCES)
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) $< -c -o $@

clean:
	@rm -rf $(OBJECTS) $(BINNAME)

compress:
	tar -caf dragonshell.tar.gz $(SOURCES) Makefile README.md
