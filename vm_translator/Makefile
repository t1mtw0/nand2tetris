CC = gcc
CFLAGS = -std=c17 -Wall

BUILD_DIR = build
HEADERS := $(shell find . -name "*.h")

default: $(BUILD_DIR)/vm_translator

$(BUILD_DIR)/vm_translator: main.c $(BUILD_DIR)/code_writer.o $(BUILD_DIR)/parser.o
	@ mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/code_writer.o: code_writer.c $(HEADERS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/parser.o: parser.c $(HEADERS)
	@ mkdir -p $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

.PHONE: clean
