CC      = clang
CFLAGS  = -Wall -Wextra -march=native
DFLAGS 	= -g3 -DMYTHRIL_DEBUG

SRC_DIR   = src
BUILD_DIR = build
BIN_DIR   = $(BUILD_DIR)/bin

MYTHRIL = $(BIN_DIR)/mythril

SRCS 	= $(shell find $(SRC_DIR) -name "*.c")
OBJECTS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TIDY_CHECKS = -*,\
			  bugprone-suspicious-*,\
			  bugprone-infinite-loop,\
			  bugprone-branch-clone,\
			  bugprone-dangling-handle,\
			  clang-analyzer-core.*,\
			  clang-analyzer-deadcode.*

.PHONY: all check clean debug test

all: $(MYTHRIL)

debug: CFLAGS += $(DFLAGS)
debug: $(MYTHRIL)

$(MYTHRIL): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

$(BIN_DIR): | $(BUILD_DIR)
	@mkdir -p $@

check:
	@clang-tidy $(SRCS) --checks='$(TIDY_CHECKS)' -- $(CFLAGS) -I$(SRC_DIR)

test:
	@cd tests && bash run_tests.sh

clean:
	@rm -rvf $(BUILD_DIR) > /dev/null
