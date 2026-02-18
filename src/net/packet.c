#include "packet.h"
#include <stddef.h>
#include <string.h>

void send_packet(ENetPeer *peer, uint8_t type, void *data, size_t data_size) {
  Packet p;
  p.type = type;
  memset(p.data, 0, PACKET_DATA_SIZE);
  size_t s = data_size > PACKET_DATA_SIZE ? PACKET_DATA_SIZE : data_size;
  memcpy(p.data, data, s);

  ENetPacket *epacket = enet_packet_create(&p, sizeof(Packet), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(peer, 0, epacket);
}
