CC=gcc
CFLAGS=-Wall -std=c17

BUILD_DIR := build
HEADERS := $(shell find . -name '*.h')

default: build/assembler

build/assembler: assembler.c $(BUILD_DIR)/parser.o $(BUILD_DIR)/code.o
	mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/parser.o: parser.c $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/code.o: code.c $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
