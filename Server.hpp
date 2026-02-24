//
// Created by pasquale on 23/02/26.
//

#ifndef RETI_DI_CALCOLATORI_SERVER_HPP
#define RETI_DI_CALCOLATORI_SERVER_HPP

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <filesystem>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> // Per usare il formato uint64_t

class Server {
public:
    // Il costruttore inizializza il socket e fa il bind
    Server(int port);
    // Il distruttore chiude il socket automaticamente
    ~Server();

    // Avvia il server in un loop infinito
    void run();

private:
    int _server_fd;
    int _port;

    // Gestisce la comunicazione con un singolo client
    void handleClient(int client_socket);
    // Invia il file binario al client
    void sendFile(int client_socket, const std::string& filename);
};

#endif //RETI_DI_CALCOLATORI_SERVER_HPP