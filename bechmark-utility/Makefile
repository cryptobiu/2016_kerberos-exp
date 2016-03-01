
SRC = $(shell find . -name *.c)

OBJ = $(SRC:%.c=%.o)

BIN = bin/benchmark

all: $(BIN)

$(BIN): $(OBJ)
	gcc -g $(OBJ) -l pthread -l krb5 -o $(BIN)

%.o: %.c
	gcc -g -c $< -o $@

clean:
	rm src/*.o
	rm bin/benchmark
