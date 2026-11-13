CC:=gcc
CFLAGS:=-Wall -Wextra -Werror

SRC_DIR:=./src
OUT_DIR:=./out
BIN_SRC_DIR:=$(SRC_DIR)/bin
BIN_SOURCES:=$(wildcard $(BIN_SRC_DIR)/*.c)
BIN_OUTPUT:=$(OUT_DIR)/acrt
COMPTIME_DEFINITIONS:= \
											 -DPROGRAM_TAG='"$(shell git describe --tags --abbrev=0 HEAD)"' \
											 -DPROGRAM_SHORT_COMMIT_HASH='"$(shell git rev-parse --short HEAD)"' \
											 -DPROGRAM_FULL_COMMIT_HASH='"$(shell git rev-parse HEAD)"' \
											 -DPROGRAM_COMMIT_DATE='"$(shell git show -s --format=%cs HEAD)"'\
											 -DPROGRAM_HOST_MACHINE='"$(shell $(CC) -dumpmachine)"'\
											 -DPROGRAM_COMPILER='"$(CC)"' \
											 -DCOMPILER_VERSION='"$(shell $(CC) -dumpversion)"'

ifdef DEBUG
	CFLAGS+=-g
endif

all: $(BIN_OUTPUT)

$(OUT_DIR):
	mkdir $@

$(BIN_OUTPUT): $(OUT_DIR)
	$(CC) $(CFLAGS) $(BIN_SOURCES) $(COMPTIME_DEFINITIONS) -o $@

.PHONY: all
