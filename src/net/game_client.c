#include "game_client.h"
// #define LOGS
#include "def.h"
#include "enet/enet.h"
#include <stdio.h>

int game_client_init(GameClient *gc, char *address, unsigned long port) {
  if(!gc || !address) return 1;
  gc->host = NULL;
  gc->peer = NULL;

  if(enet_initialize()) {
    PERROR("Failed to initialize ENet\n");
    return 1;
  }

  PINFO("Initialized ENet\n");

  gc->host = enet_host_create(NULL, 1, 1, 0, 0);
  if(!gc->host) {
    PERROR("Failed to create client host\n");
    enet_deinitialize();
    return 1;
  }

  ENetAddress eaddress;
  enet_address_set_host(&eaddress, address);
  eaddress.port = port;

  gc->peer = enet_host_connect(gc->host, &eaddress, 1, 0);
  if(!gc->peer) {
    PERROR("Couldn't connect to server\n");
    enet_host_destroy(gc->host);
    enet_deinitialize();
    return 1;
  }

  PINFO("Connecting to %x:%u\n", eaddress.host, eaddress.port);

  ENetEvent event;
  if(!(enet_host_service(gc->host, &event, CONNECTION_WAIT_MS) && event.type == ENET_EVENT_TYPE_CONNECT)) {
    PERROR("Failed to connect to server: No connection response\n");
    enet_peer_reset(gc->peer);
    enet_host_destroy(gc->host);
    enet_deinitialize();
    return 1;
  }

  for(size_t i = 0; i < MAX_PLAYERS; i++) {
    for(size_t j = 0; j < MAX_ROLLBACK; j++) {
      gc->inputs[i].inputs[j] = 0;
      gc->inputs[i].frames[j] = 0;
    }
  }

  gc->connected = 1;
  gc->started = 0;

  PINFO("Successfully connected\n");
  return 0;
}

static void game_client_store_input(GameClient *gc, uint8_t player_id, uint64_t frame_no, uint64_t inputs) {
  InputHistory *ih = &gc->inputs[player_id];
  uint64_t index = frame_no % MAX_ROLLBACK;
  uint64_t stored_frame = ih->frames[index];

  if (frame_no <= stored_frame + MAX_AHEAD) {
    ih->inputs[index] = inputs;
    ih->frames[index] = frame_no;
    PINFO("Stored inputs %lu for player %d on frame %lu\n", inputs, (int)player_id, frame_no);
  } else {
    PINFO("Frame %lu too far ahead of stored frame %lu, ignoring\n", frame_no, stored_frame);
  }
}

int game_client_get_input(GameClient *gc, uint8_t player_id, uint64_t frame_no, uint64_t *res) {
  InputHistory *ih = &gc->inputs[player_id];
  if(ih->frames[frame_no % MAX_ROLLBACK] == frame_no) {
    *res = ih->inputs[frame_no % MAX_ROLLBACK];
    return 0;
  } else return 1;
}

static void game_client_handle_input(GameClient *gc, ENetEvent event) {
  Packet *packet = (Packet *)event.packet->data;
  OtherInputPacket *input = (OtherInputPacket *)&packet->data[0];
  uint8_t id = input->player_id;
  uint64_t frame = input->frame_no;

  game_client_store_input(gc, id, frame, input->inputs);
}

static void game_client_handle_start(GameClient *gc, ENetEvent event) {
  Packet *packet = (Packet *)event.packet->data;
  StartPacket *start_packet = (StartPacket *)&packet->data[0];
  uint8_t num_players = start_packet->num_players;

  PINFO("Starting with %d players\n", (int)num_players);
  gc->num_players = num_players;
  gc->started = 1;
}

static void game_client_handle_id(GameClient *gc, ENetEvent event) {
  Packet *packet = (Packet *)event.packet->data;
  IdPacket *id_packet = (IdPacket *)&packet->data[0];
  uint8_t id = id_packet->player_id;

  gc->player_id = id;
  PINFO("Got player ID %d from server\n", (int)id);
}

static void game_client_handle_receive(GameClient *gc, ENetEvent event) {
  // Ensure the packet is the right size
  if(event.packet->dataLength != sizeof(Packet)) {
    PERROR("Invalid packet. Expected size %zu, but got %zu.\n",
        sizeof(Packet), event.packet->dataLength);
    return;
  }

  // Handle packet differently based on its type
  Packet *p = (Packet *)event.packet->data; 
  switch(p->type) {
    case PACKET_OTHER_INPUT: game_client_handle_input(gc, event); break;
    case PACKET_ID: game_client_handle_id(gc, event); break;
    case PACKET_START: game_client_handle_start(gc, event); break;
    default: PERROR("Unknown packet type %d.\n", (int)p->type);
  }
}

void game_client_process(GameClient *gc) {
  if(!gc || !gc->host) return;

  ENetEvent event;
  while(enet_host_service(gc->host, &event, POLL_MS)) {
    switch(event.type) {
      // The server sending data
      case ENET_EVENT_TYPE_RECEIVE:
        game_client_handle_receive(gc, event);
        enet_packet_destroy(event.packet);
        break;

        // Probably need to handle disconnecting and stuff
      case ENET_EVENT_TYPE_DISCONNECT:
        gc->connected = 0;
        PINFO("Server disconnected\n");
        break;
      default: break;
    }
  }
}

void game_client_send_input(GameClient *gc, uint64_t frame, uint64_t inputs) {
  if(!gc->started) return;
  InputPacket ip = (InputPacket) {
    .frame_no = frame,
    .inputs = inputs
  };

  send_packet(gc->peer, PACKET_INPUT, &ip, sizeof(InputPacket));
  PINFO("Sent inputs %lu on frame %lu\n", inputs, frame);

  // Also store for this player
  game_client_store_input(gc, gc->player_id, frame, inputs);
}

void game_client_close(GameClient *gc) {
  if(!gc) return;
  if(gc->peer) enet_peer_disconnect(gc->peer, 0);
  if(gc->host) enet_host_destroy(gc->host);
  enet_deinitialize();
}
