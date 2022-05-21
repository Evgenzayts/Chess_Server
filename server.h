#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "stdbool.h"

#define PORT 9898
#define KEY 0xF5E6

enum _Player_turn {
    PL_1_TURN,
    PL_2_TURN
};

typedef enum _Player_turn Player_turn;

struct _Server {
    int _socket;
    uint32_t _message;
    uint16_t _check_message;
    Player_turn _turn;
    struct sockaddr_in _serv_info,
            _client_1_info,
            _client_2_info,
            _current_client;
};

typedef struct _Server Server;

bool init_server(Server *my_server);

void first_connection(Server *my_server);

bool recieve_message(Server *my_server);

void send_check_msg(Server *my_server);

bool recieve_check_msg(Server *my_server);

void checked_recv(Server *my_server);

bool key_in_recv(Server *my_server);

void checked_send(Server *my_server);

void change_current_player(Server *my_server);

void _send_message(Server *my_server);

void close_server(Server *my_server);

#endif // SERVER_H
