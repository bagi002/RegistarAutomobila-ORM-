# Makefile for Server project
# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread

# Directories
SRC_DIR = .
TEST_DIR = tests

# Source files for server
SERVER_SOURCES = server.c \
                  data/data_loader.c \
                  network/network_utils.c \
                  commands/command_parser.c \
                  handlers/client_handler.c \
                  models/impl/korisnik.c \
                  models/impl/vozilo.c

# Source files for test client
CLIENT_SOURCES = $(TEST_DIR)/test_klijent.c

# Target executables
SERVER_TARGET = server
CLIENT_TARGET = $(TEST_DIR)/test_klijent

# Default target - build both server and client
all: $(SERVER_TARGET) $(CLIENT_TARGET)

# Build server
$(SERVER_TARGET): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(SERVER_SOURCES)

# Build test client
$(CLIENT_TARGET): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(CLIENT_SOURCES)

# Clean target
clean:
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET)

# Install dependencies (if needed)
install:
	@echo "No dependencies to install"

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build both server and client"
	@echo "  server  - Build only the server"
	@echo "  client  - Build only the test client"
	@echo "  clean   - Remove all built executables"
	@echo "  help    - Show this help message"

# Individual targets
server: $(SERVER_TARGET)
client: $(CLIENT_TARGET)

# Phony targets
.PHONY: all clean install help server client