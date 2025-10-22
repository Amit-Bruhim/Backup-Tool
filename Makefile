# === Makefile for Backup Program ===
# 🗂️ Builds and runs the backup utility written in C

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra

# Directories
SRC_DIR = src
BIN_DIR = bin

# Target binary name
TARGET = $(BIN_DIR)/backup

# === Build target ===
# Creates the 'bin' directory if it doesn't exist,
# then compiles the source file into the binary.
$(TARGET): $(SRC_DIR)/backup.c
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_DIR)/backup.c

# === Run target ===
# Runs the compiled program with example directories.
run: $(TARGET)
	./$(TARGET) source_dir dest_dir

# === Clean target ===
# Deletes the binary file and output directories.
clean:
	rm -rf $(BIN_DIR)
	rm -rf dest_dir

# === Default target ===
# By default, build the program.
all: $(TARGET)
