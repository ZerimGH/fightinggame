#include "game_server.h"
#define LOGS
#include "def.h"
#include "packet.h"
#include "shared.h"
#include "enet/enet.h"
#include <stdio.h>

// TODO: Like port and address in args
// Or LAN
int game_server_init(GameServer *gs) {
  if(!gs) return 1;
  // Initialize ENet
  if(enet_initialize()) {
    PERROR("Failed to initialize ENet\n");
    return 1;
  }
  PINFO("Initialized ENet\n");

  // Create server host
  ENetAddress address; 
  address.host = HOST;
  address.port = PORT;

  gs->host = enet_host_create(&address, MAX_PLAYERS, 0, 0, 0);
  if(!gs->host) {
    PERROR("Failed to create server host.\n");
    enet_deinitialize();
    return 1;
  }

  char ip[64];
  enet_address_get_host_ip(&gs->host->address, ip, sizeof(ip));
  PINFO("Created server host at %s:%u\n",
      ip,
      gs->host->address.port);

  // Initialize player list
  gs->player_count = 0;
  for(size_t i = 0; i < MAX_PLAYERS; i++) {
    gs->players[i] = (Player) {
      .connected = 0,
      .player_id = (uint8_t)i
    };
  }

  gs->running = 1;

  return 0;
}

static void game_server_handle_connect(GameServer *gs, ENetEvent event) {
  if(gs->player_count >= MAX_PLAYERS) {
    PERROR("This should never happen.\n");
    return;
  }

  Player *p = &gs->players[gs->player_count];
  p->connected = 1;
  ENetPeer *peer = event.peer;
  peer->data = (void *)p;
  p->peer = event.peer;
  gs->player_count++;

  // Send player id to the client
  IdPacket id_packet = {
    .player_id = p->player_id
  }; 
  send_packet(p->peer, PACKET_ID, &id_packet, sizeof(id_packet));

  // Broadcast game start when all players have connected 
  StartPacket start_packet = {
    .num_players = MAX_PLAYERS
  };

  if(gs->player_count == MAX_PLAYERS) {
    for(size_t i = 0; i < MAX_PLAYERS; i++) {
      Player *pl = &gs->players[i];
      if(!pl->connected) continue;
      send_packet(pl->peer, PACKET_START, &start_packet, sizeof(start_packet));
    }
  }
}

static void game_server_handle_input(GameServer *gs, ENetEvent event);

static void game_server_handle_receive(GameServer *gs, ENetEvent event) {
  // Ensure the packet is the right size
  if(event.packet->dataLength != sizeof(Packet)) {
    PERROR("Invalid packet. Expected size %zu, but got %zu.\n",
        sizeof(Packet), event.packet->dataLength);
    return;
  }

  // Handle packet differently based on its type
  Packet *p = (Packet *)event.packet->data; 
  switch(p->type) {
    case PACKET_INPUT: game_server_handle_input(gs, event); break;
    default: PERROR("Unknown packet type %d.\n", (int)p->type);
  }
}

static void game_server_handle_disconnect(GameServer *gs, ENetEvent event) {
  Player *player = event.peer->data;
  player->connected = 0;
  player->peer = NULL;
  PINFO("%x:%u disconnected, stopping server\n", event.peer->address.host, event.peer->address.port);
  gs->running = 0;
}

static void game_server_handle_input(GameServer *gs, ENetEvent event) {
  if(!event.packet || !event.peer || !event.peer->data) return;

  Packet *packet = (Packet *)event.packet->data;
  InputPacket *input = (InputPacket *)&packet->data[0];
  Player *from = (Player *)event.peer->data;

  if(input->frame_no <= from->last_frame_no) return;

  from->last_frame_no = input->frame_no;

  OtherInputPacket out_packet = {
    .player_id = from->player_id,
    .frame_no = input->frame_no,
    .inputs = input->inputs
  };

  for(size_t i = 0; i < MAX_PLAYERS; i++) {
    Player *to = &gs->players[i];
    if(!to->connected) continue;
    if(to == from) continue;

    send_packet(to->peer, PACKET_OTHER_INPUT, &out_packet, sizeof(out_packet)); 
  }
}

void game_server_process(GameServer *gs) {
  if(!gs || !gs->host) return;
  if(!gs->running) return;
  ENetEvent event;
  while(enet_host_service(gs->host, &event, POLL_MS)) {
    switch(event.type) {
      // A new client connection
      case ENET_EVENT_TYPE_CONNECT:
        game_server_handle_connect(gs, event);
        break;
        // A client sending data
      case ENET_EVENT_TYPE_RECEIVE:
        game_server_handle_receive(gs, event);
        enet_packet_destroy(event.packet);
        break;
        // A client disconnecting
      case ENET_EVENT_TYPE_DISCONNECT:
        game_server_handle_disconnect(gs, event);
        break;
        // Not sure what this is
      case ENET_EVENT_TYPE_NONE: break;
    }
  }
}

void game_server_close(GameServer *gs) {
  if (!gs || !gs->host) return;

  // Disconnect all players
  for(size_t i = 0; i < MAX_PLAYERS; i++) {
    Player *p = &gs->players[i];
    if (p->connected && p->peer) {
      enet_peer_disconnect(p->peer, 0);
      enet_host_flush((ENetHost *)gs->host);
      p->connected = 0;
      p->peer = NULL;
    }
  }

  // Destroy server
  enet_host_destroy(gs->host);
  gs->host = NULL;

  enet_deinitialize();
}
