#ifndef PACKET_H

#define PACKET_H

#include <stdint.h>
#include <enet/enet.h>

#define PACKET_DATA_SIZE 255

enum {
  PACKET_INPUT,
  PACKET_OTHER_INPUT,
  PACKET_ID,
  PACKET_START
};

typedef struct {
  uint8_t type;
  char data[PACKET_DATA_SIZE];
} __attribute__((packed)) Packet;

typedef struct {
  uint64_t frame_no;
  uint64_t inputs;
} __attribute__((packed)) InputPacket;

typedef struct {
  uint8_t player_id;
  uint64_t frame_no;
  uint64_t inputs;
} __attribute__((packed)) OtherInputPacket;

typedef struct {
  uint8_t player_id;
} __attribute__((packed)) IdPacket;

typedef struct {
  uint8_t num_players;
} __attribute__((packed)) StartPacket;

void send_packet(ENetPeer *peer, uint8_t type, void *data, size_t data_size);

#endif // PACKET_H
