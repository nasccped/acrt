CC=gcc
CFLAGS=-Wall -Wextra -Werror
DEFINED_VALUES=-D_MK_REPO_URL='"$(shell git remote get-url origin)"'

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
	$(CC) $(CFLAGS) $(DEFINED_VALUES) $(BIN_SOURCES) -o "$(BIN_OUTPUT)"

$(BIN_OUTPUT): build

.PHONY: all build
