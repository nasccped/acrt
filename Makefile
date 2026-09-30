CC=gcc
OUT_DIR=./out
SRC=./main.c
TOY=./toy.c
COMMON_FLAGS=-g -Wall -Wextra -Werror
TOY_OBJ=$(patsubst ./%.c,$(OUT_DIR)/%.so,$(TOY))
A_OUT=$(OUT_DIR)/a.out

all: $(TOY_OBJ) $(A_OUT)

$(OUT_DIR):
	mkdir "$@"

$(TOY_OBJ): build

$(A_OUT): build

build: $(OUT_DIR) $(TOY) $(SRC)
	$(CC) $(COMMON_FLAGS) -shared -fPIC $(TOY) -o $(TOY_OBJ)
	$(CC) $(COMMON_FLAGS) $(SRC) -o $(A_OUT)

.PHONY: all build
