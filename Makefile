# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -pthread

# Source directories and files
SRC_DIR = src
HEADERS_DIR = header
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(HEADERS_DIR)/*.h)

# Output directories
OUTPUT_SERVER = server
OUTPUT_CLIENT = client

# Targets
all: server client

server: $(SRC_FILES) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC_DIR)/server.c $(SRC_DIR)/books.c $(SRC_DIR)/users.c -o $(OUTPUT_SERVER)

client: $(SRC_FILES) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC_DIR)/client.c $(SRC_DIR)/books.c $(SRC_DIR)/users.c -o $(OUTPUT_CLIENT)

clean:
	rm -f $(OUTPUT_SERVER) $(OUTPUT_CLIENT)
