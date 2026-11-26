CC:=gcc
CFLAGS:=-Wall -Wextra -Werror
PROGRAM_NAME:=acrt
PROGRAM_TAG:=$(shell git describe --tags --abbrev=0 HEAD)
PROGRAM_VERSION:=$(patsubst v%,%,$(PROGRAM_TAG))
PROGRAM_MAJOR:=$(firstword $(subst ., ,$(PROGRAM_VERSION)))

ifdef DEBUG
	CFLAGS+=-g
endif

# output directories
OUT_DIR:=./out
OUT_BIN_DIR:=$(OUT_DIR)/bin
OUT_LIB_DIR:=$(OUT_DIR)/lib
OUT_EXAMPLES_DIR:=$(OUT_DIR)/examples

# runtime
RUNTIME_DIR:=./runtime
RUNTIME_SRCS:=$(wildcard $(RUNTIME_DIR)/*.c)
RUNTIME_OBJS:=$(patsubst $(RUNTIME_DIR)/%.c,$(OUT_DIR)/%.o,$(RUNTIME_SRCS))
RUNTIME_OUT:=$(OUT_BIN_DIR)/$(PROGRAM_NAME)

# lib (.so and headers)
LIB_DIR:=./src
LIB_SRCS:=$(wildcard $(LIB_DIR)/*.c)
INCLUDE_DIR:=./include
LIB_HEADER:=$(INCLUDE_DIR)/$(PROGRAM_NAME).h
LIB_HEADER_OUT:=$(subst $(INCLUDE_DIR)/%,$(OUT_LIB_DIR)/%,$(LIB_HEADER))
LIB_SO_LONG_VER:=$(OUT_LIB_DIR)/$(PROGRAM_NAME).so.$(PROGRAM_VERSION)
LIB_SO_MAJOR:=$(OUT_LIB_DIR)/$(PROGRAM_NAME).so.$(PROGRAM_MAJOR)
LIB_SO:=$(OUT_LIB_DIR)/$(PROGRAM_NAME).so

# examples
EXAMPLES_DIR:=./examples
EXAMPLES_SRCS:=$(wildcard $(EXAMPLES_DIR)/*.c)
EXAMPLES_SOS:=$(patsubst $(EXAMPLES_DIR)/%.c,$(OUT_EXAMPLES_DIR)/ex-%.so,$(EXAMPLES_SRCS))

# required compile-time definitions
COMPTIME_DEFINITIONS:= \
											 -DPROGRAM_DESCRIPTION='"Easy and ergonomic testing runtime for C apps"' \
											 -DPROGRAM_TAG='"$(PROGRAM_TAG)"' \
											 -DPROGRAM_SHORT_COMMIT_HASH='"$(shell git rev-parse --short HEAD)"' \
											 -DPROGRAM_FULL_COMMIT_HASH='"$(shell git rev-parse HEAD)"' \
											 -DPROGRAM_COMMIT_DATE='"$(shell git show -s --format=%cs HEAD)"'\
											 -DPROGRAM_HOST_MACHINE='"$(shell $(CC) -dumpmachine)"'\
											 -DPROGRAM_COMPILER='"$(CC)"' \
											 -DCOMPILER_VERSION='"$(shell $(CC) -dumpversion)"'

all: $(LIB_SO) $(RUNTIME_OUT) $(LIB_HEADER_OUT)

examples: $(EXAMPLES_SOS)
	$(RUNTIME_OUT) $^

$(OUT_DIR):
	mkdir "$@"

$(OUT_BIN_DIR): | $(OUT_DIR)
	mkdir "$@"

$(OUT_LIB_DIR): | $(OUT_DIR)
	mkdir "$@"

$(OUT_EXAMPLES_DIR): | $(OUT_DIR)
	mkdir "$@"

$(OUT_DIR)/%.o: $(RUNTIME_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) $(COMPTIME_DEFINITIONS) -c $< -o $@

$(RUNTIME_OUT): $(RUNTIME_OBJS) | $(OUT_BIN_DIR)
	$(CC) $(filter %.o,$^) -o $@

$(LIB_SO_LONG_VER): $(LIB_SRCS) | $(OUT_LIB_DIR)
	$(CC) $(CFLAGS) -shared -fPIC $(filter %.c, $^) -o $@

$(LIB_SO_MAJOR): $(LIB_SO_LONG_VER)
	ln -s $< $@

$(LIB_SO): $(LIB_SO_MAJOR)
	ln -s $< $@

$(OUT_EXAMPLES_DIR)/ex-%.so: $(EXAMPLES_DIR)/%.c | $(OUT_EXAMPLES_DIR)
	$(CC) $(CFLAGS) -shared -fPIC -I$(INCLUDE_DIR) $(filter %.so %.c,$^) -o $@

.PHONY: all examples
