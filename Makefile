CC = gcc

CFLAGS = -Wall -Wextra -pedantic -MMD -MP

LDFLAGS_LINUX = -Lextern/lib/linux -lraylib -lenet -lm -ldl -lGL -pthread
LDFLAGS_WIN = -Lextern/lib/win -lraylib -lenet -lm -lws2_32 -lgdi32 -lopengl32 -lwinmm -pthread -mwindows

TARGET_DIR = build
INCLUDES = -Isrc -Isrc/net -Isrc/game -Iextern/include

GAME_SRCS = $(wildcard src/game/*.c) src/net/game_client.c src/net/packet.c
SERVER_SRCS = src/net/game_server.c src/net/server.c src/net/packet.c

OBJ_DIR_LINUX = $(TARGET_DIR)/obj/linux
OBJ_DIR_WIN = $(TARGET_DIR)/obj/win

GAME_OBJS_LINUX = $(GAME_SRCS:%.c=$(OBJ_DIR_LINUX)/%.o)
SERVER_OBJS_LINUX = $(SERVER_SRCS:%.c=$(OBJ_DIR_LINUX)/%.o)
GAME_OBJS_WIN = $(GAME_SRCS:%.c=$(OBJ_DIR_WIN)/%.o)
SERVER_OBJS_WIN = $(SERVER_SRCS:%.c=$(OBJ_DIR_WIN)/%.o)

GAME_TARGET_LINUX = $(TARGET_DIR)/linux/game
SERVER_TARGET_LINUX = $(TARGET_DIR)/linux/server
GAME_TARGET_WIN = $(TARGET_DIR)/win/game.exe
SERVER_TARGET_WIN = $(TARGET_DIR)/win/server.exe

LIB_DIR_LINUX = extern/lib/linux
LIB_DIR_WIN = extern/lib/win

linux: $(GAME_TARGET_LINUX) $(SERVER_TARGET_LINUX)

$(GAME_TARGET_LINUX): $(GAME_OBJS_LINUX) | $(TARGET_DIR)/linux
	$(CC) $(GAME_OBJS_LINUX) $(LDFLAGS_LINUX) -o $@

$(SERVER_TARGET_LINUX): $(SERVER_OBJS_LINUX) | $(TARGET_DIR)/linux
	$(CC) $(SERVER_OBJS_LINUX) $(LDFLAGS_LINUX) -o $@

win: $(GAME_TARGET_WIN) $(SERVER_TARGET_WIN)

$(GAME_TARGET_WIN): $(GAME_OBJS_WIN) | $(TARGET_DIR)/win
	$(CC) $(GAME_OBJS_WIN) -L$(LIB_DIR_WIN) -lraylib -lenet $(LDFLAGS_WIN) -o $@

$(SERVER_TARGET_WIN): $(SERVER_OBJS_WIN) | $(TARGET_DIR)/win
	$(CC) $(SERVER_OBJS_WIN) -L$(LIB_DIR_WIN) -lenet $(LDFLAGS_WIN) -o $@

$(OBJ_DIR_LINUX)/%.o: %.c | $(OBJ_DIR_LINUX)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR_WIN)/%.o: %.c | $(OBJ_DIR_WIN)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_WIN) $(INCLUDES) -c $< -o $@

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)
$(TARGET_DIR)/linux:
	mkdir -p $(TARGET_DIR)/linux
$(TARGET_DIR)/win:
	mkdir -p $(TARGET_DIR)/win
$(OBJ_DIR_LINUX):
	mkdir -p $(OBJ_DIR_LINUX)
$(OBJ_DIR_WIN):
	mkdir -p $(OBJ_DIR_WIN)

clean:
	rm -rf $(TARGET_DIR)

.PHONY: linux win clean
