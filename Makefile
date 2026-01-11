#-- Makefile Usage: default release build --------
# > make      		       # make release
# > make build             # release build qc
# > make build MODE=debug  # debug build qc
# > make test              # qk tests
# > make test-c            # unit-tests
# > make release           # release build qc + tests-c
# > make debug             # debug build qc + tests-c
# > make clean             # remove build/files
#-------------------------------------------------

EXE :=
CC := gcc
QC := qc
CFLAGS := -Isrc/include -Isrc -Wall -Wno-missing-braces -Wno-char-subscripts
LDFLAGS :=

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    PLATFORM := linux
endif
ifeq ($(UNAME_S),Darwin)
    PLATFORM := macos
endif
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
    EXE := .exe
	CC := $(CC)$(EXE)
	QC := $(QC)$(EXE)
endif

# Directories
BUILD_DIR := build
TEST_BUILD := $(BUILD_DIR)/tests
UT_BUILD := $(BUILD_DIR)/unit_tests

# Source files
SRCS := $(shell find src -name '*.c')
TEST_QK := $(wildcard tests/*.qk)
UT_SRCS := unit-tests/main.c \
   $(wildcard src/*/*.c) $(wildcard src/*/*/*.c) $(wildcard src/*/*/*/*.c) \
   $(wildcard unit-tests/*/*.c) $(wildcard unit-tests/*/*/*.c)

# Output executable
OUT := ./$(QC)
MODE ?= release

.PHONY: all build debug release test test-c clean
# make default is make all
.DEFAULT_GOAL := all

# Ensure build directories exist
$(shell mkdir -p $(BUILD_DIR) $(TEST_BUILD) $(UT_BUILD))

# Build main program
build: $(SRCS)
ifeq ($(shell echo $(MODE) | tr '[:upper:]' '[:lower:]'),debug)
	@echo "------ Debug build qc ------"
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -g -ggdb -DDEBUG -o $(BUILD_DIR)/$(QC)
else
	@echo "------ Release build qc ------"
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -O2 -o $(BUILD_DIR)/$(QC)
endif
	@cp $(BUILD_DIR)/$(QC) $(OUT)

# Build everything
all: build test-c 

# test .qk using current ./qc
test:
	@echo "------ qk tests ------"
	@if [ ! -f "$(OUT)" ]; then \
		$(MAKE) "build"; \
	fi
	@mkdir -p $(TEST_BUILD)
	@for file in $(TEST_QK); do \
		base=$${file%.qk}; \
		outname=$(TEST_BUILD)/$${base##*/}$(EXE); \
		echo "Compiling $$file -> $$outname.c"; \
		if $(OUT) "$$file" -o "$(TEST_BUILD)/$${base##*/}.c" -l "$(PWD)"; then \
			echo "Building $$outname"; \
			$(CC) $(CFLAGS) -g $(LDFLAGS) "$(TEST_BUILD)/$${base##*/}.c" -o $$outname; \
		else \
			echo "!!! ERROR: $$file failed to compile, skipping"; \
		fi \
	done

# unit-tests
test-c:
	@echo "------ unit-tests ------"
	@mkdir -p $(UT_BUILD)
	@if [ ! -f "$(UT_BUILD)/unit-tests$(EXE)" ]; then \
		echo "Compiling unit-tests"; \
		$(CC) $(CFLAGS) -g $(LDFLAGS) $(UT_SRCS) -o $(UT_BUILD)/unit-tests$(EXE); \
	fi
	@echo "Running unit tests..."
	@$(UT_BUILD)/unit-tests$(EXE)

# only release build qc, tests all debug build
release:
	$(MAKE) build MODE=release
	$(MAKE) test-c

# debug build qc
debug:
	$(MAKE) build MODE=debug
	$(MAKE) test-c

# Clean generated files
clean:
ifeq ($(PLATFORM),windows)
	del /Q $(BUILD_DIR)\* $(OUT)
else
	-rm -rf $(BUILD_DIR)/* $(OUT)
endif

