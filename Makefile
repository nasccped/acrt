CC=gcc
FLAGS=-Wall -Wextra
SRC_DIR=./src
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
TEST_DIR=./tests
TEST_FILES=$(wildcard $(TEST_DIR)/*.c)
TEST_BINS=$(patsubst $(TEST_DIR)/%.c,$(OUT_DIR)/%.exe,$(TEST_FILES))
OUT_DIR=./out

# build test cases
test-build: $(TEST_BINS)

# run test cases
test-run: $(TEST_BINS)
	$(foreach test,$^,./$(test))

# pattern build recipe
$(OUT_DIR)/%.exe: $(TEST_DIR)/%.c $(SRC_FILES)
	$(CC) $(FLAGS) $^ -o $@

.PHONY: test-build test-run
