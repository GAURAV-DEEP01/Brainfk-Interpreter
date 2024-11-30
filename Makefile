GXX = gcc
CFLAGS = -Wall -Werror -g
SRC = brainfk.c
OUT = bfk

all: build 

build: $(SRC)
	$(GXX) $(SRC) -o $(OUT)

clean:
	rm -f $(SRC) 