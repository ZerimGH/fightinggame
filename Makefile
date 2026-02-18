CC = gcc
CFLAGS = -Wall -Wextra -pedantic -pthread -MMD -MP
LDFLAGS = -lm -ldl -lGL -pthread

TARGET_DIR = build
OBJ_DIR = $(TARGET_DIR)/obj

GAME_SRCS = $(wildcard src/game/*.c) src/net/game_client.c src/net/packet.c

SERVER_SRCS = src/net/game_server.c src/net/server.c src/net/packet.c

GAME_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(GAME_SRCS))
SERVER_OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SERVER_SRCS))

GAME_TARGET = $(TARGET_DIR)/game
SERVER_TARGET = $(TARGET_DIR)/server

RAYLIB_DIR = extern/raylib
RAYLIB_BUILD_DIR = $(RAYLIB_DIR)/build/raylib
RAYLIB_LIB = $(RAYLIB_BUILD_DIR)/libraylib.a
RAYLIB_INCLUDE = $(RAYLIB_DIR)/src

ENET_DIR = extern/enet
ENET_BUILD_DIR = $(ENET_DIR)/build
ENET_LIB = $(ENET_BUILD_DIR)/libenet.a
ENET_INCLUDE = $(ENET_DIR)/include

CINCLUDES = -I$(RAYLIB_INCLUDE) -I$(ENET_INCLUDE) -Isrc -Isrc/game -Isrc/net


all: $(GAME_TARGET) $(SERVER_TARGET)


$(ENET_LIB):
	cd $(ENET_DIR) && cmake -B build -DENET_SHARED=0 -DCMAKE_BUILD_TYPE=Release
	cd $(ENET_DIR) && cmake --build build

$(RAYLIB_LIB):
	mkdir -p $(RAYLIB_BUILD_DIR)
	cd $(RAYLIB_BUILD_DIR) && cmake .. -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release
	cd $(RAYLIB_BUILD_DIR) && make raylib


$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)


$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CINCLUDES) -c $< -o $@


$(GAME_TARGET): $(GAME_OBJS) $(RAYLIB_LIB) $(ENET_LIB) | $(TARGET_DIR)
	$(CC) $(GAME_OBJS) $(RAYLIB_LIB) $(ENET_LIB) $(LDFLAGS) -o $@

$(SERVER_TARGET): $(SERVER_OBJS) $(ENET_LIB) | $(TARGET_DIR)
	$(CC) $(SERVER_OBJS) $(ENET_LIB) -pthread -o $@

-include $(GAME_OBJS:.o=.d)
-include $(SERVER_OBJS:.o=.d)

game: $(GAME_TARGET)
server: $(SERVER_TARGET)

clean:
	rm -rf $(TARGET_DIR)
	cd $(RAYLIB_BUILD_DIR) && make clean || true
	cd $(ENET_BUILD_DIR) && make clean || true

.PHONY: all clean run game server

