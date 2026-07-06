CC=gcc
FLAGS=-Wall -Wextra
SRC_DIR=./src
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
EXAMPLE_DIR=./examples
EXAMPLE_FILES=$(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLE_BINS=$(patsubst $(EXAMPLE_DIR)/%.c,$(OUT_DIR)/exmp-%.exe,$(EXAMPLE_FILES))
TEST_DIR=./tests
TEST_FILES=$(wildcard $(TEST_DIR)/*.c)
TEST_BINS=$(patsubst $(TEST_DIR)/%.c,$(OUT_DIR)/test-%.exe,$(TEST_FILES))
OUT_DIR=./out

all:
	@echo build-examples: build all examples/files.c to out/ directory with exmp prefix
	@echo build-tests: build all tests/files.c to out/ directory with test prefix

# examples requirements
build-examples: $(OUT_DIR) $(EXAMPLE_BINS)

build-tests: $(OUT_DIR) $(TEST_BINS)

# example build recipes
$(OUT_DIR)/exmp-%.exe: $(EXAMPLE_DIR)/%.c $(SRC_FILES)
	$(CC) $(FLAGS) -Werror -g $^ -o $@

# test build recipes
$(OUT_DIR)/test-%.exe: $(TEST_DIR)/%.c $(SRC_FILES)
	$(CC) $(FLAGS) -g $^ -o $@

# if not out dir
$(OUT_DIR):
	mkdir $@

.PHONY: build-examples build-tests
