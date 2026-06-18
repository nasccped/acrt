CC=gcc
FLAGS=-Wall -Wextra
SRC_DIR=./src
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
EXAMPLE_DIR=./examples
EXAMPLE_FILES=$(wildcard $(EXAMPLE_DIR)/*.c)
EXAMPLE_BINS=$(patsubst $(EXAMPLE_DIR)/%.c,$(OUT_DIR)/exmp-%.exe,$(EXAMPLE_FILES))
OUT_DIR=./out

# examples requirements
build-example: $(OUT_DIR) $(EXAMPLE_BINS)

# example build recipes
$(OUT_DIR)/exmp-%.exe: $(EXAMPLE_DIR)/%.c $(SRC_FILES)
	$(CC) $(FLAGS) -g $^ -o $@

# if not out dir
$(OUT_DIR):
	mkdir $@

.PHONY: build-example
