# CLion source config
SRCS := $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c) \
	unit-tests/main.c $(wildcard unit-tests/*/*.c)
CFLAGS := -Isrc/include -Isrc -Wall -Wno-missing-braces -Wno-char-subscripts

BSRCS := $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c)
UT_SRCS := unit-tests/main.c $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c) \
	$(wildcard unit-tests/*/*.c)
OUT = qc

build: $(SRCS)
	$(CC) $(CFLAGS) $(BSRCS) -o $(OUT)

build-debug: $(SRCS)
	$(CC) $(CFLAGS) $(BSRCS) -o $(OUT) -g -ggdb -DEBUG

all: build

testing: unit-tests/main.c
	$(CC) $(CFLAGS) $(UT_SRCS) -o unit-tests/unit-tests -g

clean:
	rm $(OUT)