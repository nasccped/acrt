CC=gcc
CFLAGS=-Wall -Wextra -Werror
DEFINED_VALUES=-D_MK_REPO_URL='"$(shell git remote get-url origin)"' \
							 -D_MK_PROJ_TAG='"$(shell git describe --tags --abbrev=0)"' \
							 -D_MK_PROJ_SHORT_COMMIT_HASH='"$(shell git rev-parse --short HEAD)"' \
							 -D_MK_PROJ_FULL_COMMIT_HASH='"$(shell git rev-parse HEAD)"' \
							 -D_MK_PROJ_COMMIT_DATE='"$(shell git log -1 --format=%cs)"' \
							 -D_MK_PROGRAM_TARGET_MACHINE='"$(shell $(CC) -dumpmachine)"' \
							 -D_MK_COMPILER_VERSION='"$(shell $(CC) -dumpversion)"'

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
