CC = gcc
CFLAGS = -Wall -std=c17

BUILD_DIR = build
OBJECTS = $(shell find . -name "*.h")

default: $(BUILD_DIR)/jack_compiler

$(BUILD_DIR)/jack_compiler: jack_compiler.c $(BUILD_DIR)/compilation_engine.o $(BUILD_DIR)/tokenizer.o $(BUILD_DIR)/table.o $(BUILD_DIR)/vm_writer.o
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/compilation_engine.o: compilation_engine.c $(OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/tokenizer.o: tokenizer.c $(OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/table.o: table.c $(OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/vm_writer.o: vm_writer.c $(OBJECTS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@ rm -rf $(BUILD_DIR)

.phony: clean
