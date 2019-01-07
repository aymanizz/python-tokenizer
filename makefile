CFLAGS := -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter
BIN_DIR := bin

ifeq ($(MODE),debug)
	CFLAGS += -O0 -DDEBUG -g
endif

# Targets

all:
	@ echo "building tokenizer..."
	@ mkdir -p $(BIN_DIR)
	@ $(CC) $(CFLAGS) src/*.c -o $(BIN_DIR)/tokenize

test: $(BIN_DIR)/test
	@echo "running tests..."
	@ $(BIN_DIR)/test


$(BIN_DIR)/tester: test/test_*.c
	@ echo "building tests runner..."
	@ mkdir -p $(BIN_DIR)
	@ $(CC) $(CFLAGS) -I. lib/munit/munit.c test/test_*.c -o $(BIN_DIR)/test

clean:
	@ echo "removing binaries directory..."
	@ $(RM) -rf $(BIN_DIR)
	@ echo "done."

.PHONY: default test clean