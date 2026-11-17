CC:=gcc
CFLAGS:=-Wall -Wextra -Werror

ifdef DEBUG
	CFLAGS+=-g
endif

# core paths
RUNTIME_DIR:=./runtime
OUT_DIR:=./out

# runtime related fields
RUNTIME_SRCS:=$(wildcard $(RUNTIME_DIR)/*.c)
RUNTIME_OBJS:=$(patsubst $(RUNTIME_DIR)/%.c,$(OUT_DIR)/%.o,$(RUNTIME_SRCS))
RUNTIME_OUT:=$(OUT_DIR)/acrt

# required compile-time definitions
COMPTIME_DEFINITIONS:= \
											 -DPROGRAM_DESCRIPTION='"Easy and ergonomic testing runtime for C apps"' \
											 -DPROGRAM_TAG='"$(shell git describe --tags --abbrev=0 HEAD)"' \
											 -DPROGRAM_SHORT_COMMIT_HASH='"$(shell git rev-parse --short HEAD)"' \
											 -DPROGRAM_FULL_COMMIT_HASH='"$(shell git rev-parse HEAD)"' \
											 -DPROGRAM_COMMIT_DATE='"$(shell git show -s --format=%cs HEAD)"'\
											 -DPROGRAM_HOST_MACHINE='"$(shell $(CC) -dumpmachine)"'\
											 -DPROGRAM_COMPILER='"$(CC)"' \
											 -DCOMPILER_VERSION='"$(shell $(CC) -dumpversion)"'

all: $(RUNTIME_OUT)

$(OUT_DIR):
	mkdir "$@"

# runtime executable build recipe
$(RUNTIME_OUT): $(RUNTIME_OBJS)
	$(CC) $^ -o $@

# runtime-objs build recipe
$(OUT_DIR)/%.o: $(RUNTIME_DIR)/%.c $(OUT_DIR)
	$(CC) $(CFLAGS) $(COMPTIME_DEFINITIONS) -c $< -o $@

.PHONY: all
