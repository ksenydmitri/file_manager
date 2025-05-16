# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Isrc/core -Isrc/ui -Isrc/operations -Isrc/utils -Isrc/config -Isrc/error -std=c11
LDFLAGS = -lncurses -lpanel -lcrypto

# Директории
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Поиск исходных файлов
SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/file_manager

# Правила
.PHONY: all clean install debug

all: $(TARGET)

debug: CFLAGS += -g -DDEBUG -O0
debug: clean all

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

install: $(TARGET)
	install -Dm755 $(TARGET) /usr/local/bin/file_manager

# Зависимости для основных модулей
$(OBJ_DIR)/core/main.o: include/types.h include/constants.h
$(OBJ_DIR)/ui/ui.o: src/ui/ui.h include/types.h
$(OBJ_DIR)/operations/file_ops.o: src/operations/file_ops.h include/enums.h