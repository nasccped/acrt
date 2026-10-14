CC ?= gcc
CFLAGS ?= -Wall -Wextra -Werror

ifdef DEBUG
	CFLAGS += -g
endif

SRC := ./src
OUT := ./out
ACRT := $(OUT)/acrt
ACRT_SRCS := $(wildcard $(SRC)/*.c)

all: $(ACRT)

$(OUT):
	mkdir "$@"

$(ACRT): $(OUT)
	$(CC) $(CFLAGS) $(ACRT_SRCS) -o "$@"

.PHONY: all
