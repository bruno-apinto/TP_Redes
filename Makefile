CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

CLIENT_OBJS = $(OBJ_DIR)/client.o $(OBJ_DIR)/FLIP.o
SERVER_OBJS = $(OBJ_DIR)/server.o $(OBJ_DIR)/FLIP.o

TARGET_CLIENT = $(BIN_DIR)/client
TARGET_SERVER = $(BIN_DIR)/server

.PHONY: all clean

all: $(TARGET_CLIENT) $(TARGET_SERVER)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET_CLIENT): $(CLIENT_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(TARGET_SERVER): $(SERVER_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
