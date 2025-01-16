CC := gcc

FLAGS := -g -std=gnu99 -lc

SRC := $(wildcard *.c)

OBJ := $(SRC:.c=.o)

BIN := kruisies.out

%.o:%.c 
	$(CC) $(FLAGS) -c $< -o $@

all: $(OBJ)
	$(CC) $(FLAGS) $^ -o $(BIN) 

client:
	./$(BIN) $@
server:
	./$(BIN) $@

clean:
	rm *.o $(BIN)
