#ifndef TCP_PROTOCOL_RECEIVER_H
#define TCP_PROTOCOL_RECEIVER_H

#include "../Packet/Packet.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>

using namespace std;


class Receiver {

public:
    explicit Receiver();

    static Packet receive_packet(int socket_fd, struct sockaddr_in &socket_address);
    static Ack_Packet receive_ack_packet(int socket_fd, struct sockaddr_in socket_address, int& status, int TIMEOUT=1);
    static Ack_Server_Packet receive_ack_server_packet(int socket_fd, struct sockaddr_in socket_address);
};

#endif //TCP_PROTOCOL_RECEIVER_H