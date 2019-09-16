
CC = g++
CCFLAGS = -Wall -std=c++17
SRC = ./src/
OBJ = ./obj/
HEADER = ./include/

dragonshell: $(OBJ)dragonshell.o
	$(CC) $(CCFLAGS) $^ -o $@

$(OBJ)dragonshell.o: $(SRC)dragonshell.cc
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) $^ -c -o $@

clean:
	@rm dragonshell
	@rm -rf $(OBJ)

