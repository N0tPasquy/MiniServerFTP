//
// Created by pasquale on 24/02/26.
//

#ifndef RETI_DI_CALCOLATORI_CLIENT_HPP
#define RETI_DI_CALCOLATORI_CLIENT_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <cstdint> // Per usare il formato uint64_t

class Client {
public:
    Client(const std::string& ip, int port);
    ~Client();

    // Metodo principale per scaricare un file
    bool downloadFile(const std::string& filename);

private:
    int _sock_fd;
    std::string _server_ip;
    int _server_port;

    bool connectToServer();
};

#endif //RETI_DI_CALCOLATORI_CLIENT_HPP