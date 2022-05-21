#include "server.h"
#include "time.h"

bool init_server(Server *my_server)
{    
    my_server->_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (my_server->_socket < 0)
        return false;

    memset(&my_server->_serv_info, 0, sizeof(my_server->_serv_info));
    memset(&my_server->_client_1_info, 0, sizeof(my_server->_client_1_info));
    memset(&my_server->_client_2_info, 0, sizeof(my_server->_client_2_info));
    memset(&my_server->_current_client, 0, sizeof(my_server->_current_client));

    my_server->_serv_info.sin_family = AF_INET;
    my_server->_serv_info.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    my_server->_serv_info.sin_port = htons(PORT);


    int check_bind = bind(my_server->_socket,
                          (const struct sockaddr *) &my_server->_serv_info,
                          sizeof(my_server->_serv_info));

    if (check_bind)
        return false;

    my_server->_current_client = my_server->_client_1_info;
    my_server->_turn = PL_1_TURN;

    return true;
}

void first_connection(Server *my_server)
{
    checked_recv(my_server);
    my_server->_client_1_info = my_server->_current_client;
    my_server->_message = 1;
    checked_send(my_server);

    change_current_player(my_server);

    checked_recv(my_server);
    my_server->_client_2_info = my_server->_current_client;
    my_server->_message = 2;
    checked_send(my_server);

    change_current_player(my_server);

    my_server->_message = 1;
    checked_send(my_server);
}

bool recieve_message(Server *my_server)
{
    unsigned int size = sizeof(my_server->_current_client);

    int len_message = recvfrom(my_server->_socket,
                               (char*) &my_server->_message,
                               sizeof (uint32_t),
                               MSG_WAITALL,
                               (struct sockaddr *) &my_server->_current_client,
                               &size);

    if (len_message < 0)
        return false;

    return true;
}

void change_current_player(Server *my_server)
{
    if (my_server->_turn == PL_1_TURN) {
        my_server->_current_client = my_server->_client_2_info;
        my_server->_turn = PL_2_TURN;
    } else {
        my_server->_current_client = my_server->_client_1_info;
        my_server->_turn = PL_1_TURN;
    }

}

void _send_message(Server *my_server)
{
    unsigned int size = sizeof(my_server->_current_client);
    sendto(my_server->_socket,
           (char*) &my_server->_message,
           sizeof (uint32_t),
           0,
           (const struct sockaddr *)
           &my_server->_current_client,
           size);
}

void close_server(Server *my_server)
{
    close(my_server->_socket);
}

void checked_recv(Server *my_server)
{
    while(!key_in_recv(my_server))
        continue;

    send_check_msg(my_server);
}

void send_check_msg(Server *my_server)
{
    my_server->_check_message = KEY;

    unsigned int size = sizeof(my_server->_current_client);
    sendto(my_server->_socket,
           (char*) &my_server->_check_message,
           sizeof (uint16_t),
           0,
           (const struct sockaddr *)
           &my_server->_current_client,
           size);
}

void checked_send(Server *my_server)
{
    bool is_recieved = false;
    do {
        _send_message(my_server);

        for (size_t i = 0; i < 100; i++) {
            if (recieve_check_msg(my_server) && my_server->_check_message == KEY) {
                is_recieved = true;
                break;
            }
            usleep(10000);
        }
    } while (!is_recieved);
}

bool recieve_check_msg(Server *my_server)
{
    unsigned int size = sizeof(my_server->_current_client);
    int len_message = recvfrom(my_server->_socket,
                               (char*) &my_server->_check_message,
                               sizeof (uint16_t),
                               MSG_DONTWAIT,
                               (struct sockaddr *) &my_server->_current_client,
                               &size);

    if (len_message < 0)
        return false;

    return true;
}

bool key_in_recv(Server *my_server)
{
    while(!recieve_message(my_server))
        continue;

    uint16_t key_from_msg = my_server->_message >> 16;
    if (key_from_msg == KEY) {
        return true;
    } else {
        return false;
    }
}
