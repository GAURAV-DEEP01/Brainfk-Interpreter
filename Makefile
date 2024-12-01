GXX = gcc
CFLAGS = -Wall -Werror -g
SRC = brainfk.c
OUT = bfk

all: build 

build: clean $(SRC)
	$(GXX) $(SRC) $(CFLAGS) -o $(OUT)

clean:
	rm -f $(OUT) 

.PHONY: all clean