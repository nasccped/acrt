CC:=gcc
CFLAGS:=-Wall -Wextra -Werror

# add debug flag if on debug mode
ifdef DEBUG
	CFLAGS+=-g
endif

SRC:=./src
OUT:=./out
PROGRAM_NAME:=acrt
ACRT:=$(OUT)/$(PROGRAM_NAME)
ACRT_SRCS:=$(wildcard $(SRC)/*.c)
DEFINITIONS:=-DPROGRAM_NAME='"$(PROGRAM_NAME)"' \
						 -DPROGRAM_TAG='"$(shell git describe --tags --abbrev=0)"' \
						 -DPROGRAM_SHORT_COMMIT_HASH='"$(shell git rev-parse --short HEAD)"' \
						 -DPROGRAM_LONG_COMMIT_HASH='"$(shell git rev-parse HEAD)"' \
						 -DPROGRAM_COMMIT_DATE='"$(shell git log -1 --format=%cs)"' \
						 -DPROGRAM_TARGET_MACHINE='"$(shell $(CC) -dumpmachine)"' \
						 -DPROGRAM_COMPILER='"$(CC) $(shell $(CC) -dumpversion)"'

all: $(ACRT)

$(OUT):
	mkdir "$@"

$(ACRT): build

build: $(OUT)
	$(CC) $(DEFINITIONS) $(CFLAGS) $(ACRT_SRCS) -o $(ACRT)

.PHONY: all build
