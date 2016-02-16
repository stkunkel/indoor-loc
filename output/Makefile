CC=gcc
CFLAGS= -g -Wall
# DEPS = hellomake.h
SRC = unpackData.c
OBJ = $(SRC:.c=.o)
EXEC = unpackData

all: $(EXEC)
	
$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $<
	
clean:
	rm $(OBJ) $(EXEC)