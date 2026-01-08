OUT = qc
SRCS := $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c)
CFLAGS = -I./src/include -Wall -g -ggdb -Wno-missing-braces -Wno-char-subscripts

build: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT)

build-debug: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUT) -g -DEBUG

all: build

test: build
	./qc test/main.qk -o test/main.c
	$(CC) test/main.c -o test/main -Wno-parentheses-equality
	./test/main

clean:
	rm $(OUT)