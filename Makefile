CC:=gcc
CFLAGS:=-Wall -Wextra -Werror

SRC_DIR:=./src
OUT_DIR:=./out
BIN_SRC_DIR:=$(SRC_DIR)/bin
BIN_SOURCES:=$(wildcard $(BIN_SRC_DIR)/*.c)
BIN_OUTPUT:=$(OUT_DIR)/acrt

ifdef DEBUG
	CFLAGS+=-g
endif

all: $(BIN_OUTPUT)

$(OUT_DIR):
	mkdir $@

$(BIN_OUTPUT): $(OUT_DIR)
	$(CC) $(CFLAGS) $(BIN_SOURCES) -o $@

.PHONY: all
