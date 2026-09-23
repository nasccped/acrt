# commands and flags
CC=gcc
CFLAGS=-Wall -Wextra -Werror

# directories
SRC_DIR=./src
OUT_DIR=./out
OUT_BIN_DIR=$(OUT_DIR)/bin

# files
SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
BIN_FILE=$(OUT_BIN_DIR)/acrt

# build out dir if not exists
$(OUT_DIR):
	mkdir "$@"

# build out/bin dir if not exists
$(OUT_BIN_DIR): $(OUT_DIR)
	mkdir "$@"

# builds the final binary over src/main.c file
build-bin: $(OUT_BIN_DIR) $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(BIN_FILE)

.PHONY: build-bin
