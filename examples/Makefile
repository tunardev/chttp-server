# Compilation command
CC = gcc

# Target: The default target is set to 'all'
all: main

# Target: Compiles the source file 'main.c' to create the 'main' program
main: main.c
	$(CC) ../http_server.c main.c -o main

# Target: Runs the program
run: main
	./main

# Clean target: Removes generated files after compilation
clean:
	rm -f main

# 'make clean' command cleans the project (removes generated files)
.PHONY: clean

# 'make run' command runs the program
.PHONY: run
