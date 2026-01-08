OUT = qc
recursive_wildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2)) $(wildcard $1$2)
SRCS := $(call recursive_wildcard,./,*.c)
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