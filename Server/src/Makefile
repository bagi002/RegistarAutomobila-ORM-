# Makefile for Server project
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
TARGET = server
SRCDIR = .
SOURCES = server.c \
          data/data_loader.c \
          network/network_utils.c \
          commands/command_parser.c \
          handlers/client_handler.c \
          models/impl/korisnik.c \
          models/impl/vozilo.c

# Default target
all: $(TARGET)

# Link the target
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

# Clean target
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean