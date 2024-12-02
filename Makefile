GXX = gcc
CFLAGS = -Wall -Werror -g

SRC = brainfk.c
OUT = bfk

.PHONY: all
all: build

build: clean $(SRC)
	$(GXX) $(SRC) $(CFLAGS) -o $(OUT)

.PHONY: clean
clean:
	rm -f $(OUT)

.PHONY: run
run: build
	./$(OUT)
