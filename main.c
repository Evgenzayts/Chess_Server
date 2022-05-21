#include "server.h"

int main()
{
    Server *my_server = (Server *) malloc(sizeof(Server));
    if (!my_server)
        return -1;

    if (!init_server(my_server))
        return -1;
    //perror("Инициализация прошла");

    first_connection(my_server);
    //perror("Первое соединение прошло");

    while(1) {
        checked_recv(my_server); // получение с отправкой проверочного сообщения
        change_current_player(my_server);
        checked_send(my_server);
    }

    close_server(my_server);
    free(my_server);

    return 0;
}

