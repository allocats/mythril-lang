CC      = clang
CFLAGS  = -Wall -Wextra -march=native

SRC_DIR   = src
BUILD_DIR = build
BIN_DIR   = $(BUILD_DIR)/bin

MYTHRIL = $(BIN_DIR)/mythril

SRCS = $(wildcard $(SRC_DIR)/*.c) 				\
       $(wildcard $(SRC_DIR)/arena/*.c) 		\
       $(wildcard $(SRC_DIR)/diagnostics/*.c) 	\
       $(wildcard $(SRC_DIR)/hash/*.c) 			\
       $(wildcard $(SRC_DIR)/lexer/*.c)

OBJECTS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

all: $(MYTHRIL)

$(MYTHRIL): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

$(BIN_DIR): | $(BUILD_DIR)
	@mkdir -p $@

.PHONY: clean
clean:
	@rm -rvf $(BUILD_DIR) > /dev/null
