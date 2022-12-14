#include "Server.h"
#include "../File Handler/FileReader.h"
#include "../SR/SR_Sender.h"

Server::Server(string server_conf_file_dir) {
    freopen(server_conf_file_dir.c_str(), "r", stdin);
    cin >> server_port_number >> maximum_window_size >> random_seed >> packet_loss_prob;
    active_clients = 0;
}

void Server::init_server() {
    struct sockaddr_in address;
    address.sin_family = AF_INET; //IPv$
    // Creating socket file descriptor for the server
    if ((server_socket_fd = socket(address.sin_family, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    // Check if it is used and forcefully attaching socket to our desired address and port
    if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt Error");
        exit(EXIT_FAILURE);
    }
    /*
     * build the server's Internet address
     */
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(3000);
    int ret = bind(server_socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (ret < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("bind to address 0.0.0.0\n");
}

void Server::start_server(int strategy_option,Queue *queue, char *buffer) {
    init_server();
    printf("Server is waiting for clients\n");
        /* server main loop */
        printf("Successfully Connected with a Client\n");
        struct sockaddr_in client_address;
        // receive the file name
        memset(&client_address, 0, sizeof(client_address));

        // Process the packet according to the RED algorithm
        red(queue, buffer);

        Packet packet = Receiver::receive_packet(server_socket_fd, client_address);
        printf("packet recevied.\n");
        // checking the file
        cout << packet.data <<endl;
        string file_name(packet.data, packet.len);
        FileReader file_reader(file_name.c_str(), CHUNK_SIZE);

        int number_of_packets = ceil(file_reader.get_file_size() * 1.0 / CHUNK_SIZE);
        // sending the ack with the number of packets which will be sended

        Sender sender = Sender(client_address);
        Ack_Server_Packet server_ack_packet;
        server_ack_packet.packets_numbers = number_of_packets;
        sender.send_server_ack(server_ack_packet, server_socket_fd);
        cout << "file size is " << file_reader.get_file_size() << endl;
        cout << "transfering " << ceil(1.0 * file_reader.get_file_size() / CHUNK_SIZE) << " packet" <<endl;
        // call the desired method to send the file
        file_reader.close();
        auto start = std::chrono::high_resolution_clock::now();
        // selective repeat
        SR_Sender SR(server_socket_fd, file_name.c_str(), packet_loss_prob, random_seed, client_address);
        SR.sendFile();

        printf("Finished Client\n");
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
}