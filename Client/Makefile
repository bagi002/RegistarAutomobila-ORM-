CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I src/
TARGET = client
SRCDIR = src
SOURCES = $(SRCDIR)/main/client.c $(SRCDIR)/handler/handler.c $(SRCDIR)/menu/menu.c $(SRCDIR)/parser/parser.c $(SRCDIR)/network/network.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

debug: CFLAGS += -DDEBUG -g
debug: $(TARGET)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all debug clean install
