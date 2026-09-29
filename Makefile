CC=gcc
CFLAGS=-Wall -Wextra -Werror

ifdef DEBUG
	CFLAGS+=-g
endif

SRC_DIR=./src
OUT_DIR=./out
BIN_DIR=$(OUT_DIR)/bin

BIN_SOURCES=$(wildcard $(SRC_DIR)/*.c)
BIN_OUTPUT=$(BIN_DIR)/acrt

all: $(BIN_OUTPUT)

$(OUT_DIR):
	mkdir "$@"

$(BIN_DIR): $(OUT_DIR)
	mkdir "$@"

build: $(BIN_DIR)
	$(CC) $(CFLAGS) $(BIN_SOURCES) -o "$(BIN_OUTPUT)"

$(BIN_OUTPUT): build

.PHONY: all build
