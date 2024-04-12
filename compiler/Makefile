CC = gcc
CFLAGS = -Wall -std=c17

BUILD_DIR = build
OBJECTS = $(shell find . -name "*.h")

default: $(BUILD_DIR)/jack_analyzer

$(BUILD_DIR)/jack_analyzer: jack_analyzer.c $(BUILD_DIR)/compilation_engine.o $(BUILD_DIR)/tokenizer.o
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/compilation_engine.o: compilation_engine.c $(OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/tokenizer.o: tokenizer.c $(OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@ rm -rf $(BUILD_DIR)

.phony: clean
