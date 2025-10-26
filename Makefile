# Makefile pour t3c
CC = gcc
CFLAGS = -Wall
LDFLAGS = -lcrypto
SRCS = src/main.c src/hash_utils.c src/mode_generate.c src/mode_lookup.c
BIN = t3c

all: $(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(BIN)
