#define MAX_PLAYERS 2 // The number of players that can join a game
#define FRAMERATE 60 // The framerate the game will run at 
#define POLL_MS ((1000 / (FRAMERATE))) // The time allowed for polling the server
#define CONNECTION_WAIT_MS 10000 // The time allowed waiting for a connection response
#define HOST ENET_HOST_ANY // Server host
#define PORT 58008 // Server port
#define MAX_ROLLBACK 8 // The number of inputs history to store
