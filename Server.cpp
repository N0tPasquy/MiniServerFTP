//
// Created by pasquale on 24/02/26.
//
#include "Server.hpp"
#include <fstream>

using namespace std;

Server::Server(int port) : _port(port) {
    //  Creo il socket
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0) throw runtime_error("Errore creazione socket");

    /*  Setto il socket in modo che anche nel caso in cui la porta sia in stato di attesa,
     *  mi permetta di riutilizzarla subito
    */
    int opt = 1;
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //  Carta di identita' del server
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(_port);

    //  Con il bind collego il socket alla porta che ho ricevuto in ingresso dal costruttore
    if (bind(_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        throw runtime_error("Errore nel binding della porta");
    }
}

Server::~Server() {
    /* Prima di eliminare il socket e liberare la porta,
     * controllo che questo sia stato creato correttamente
    */
    if (_server_fd >= 0) close(_server_fd);
}

void Server::run() {
    listen(_server_fd, 10);
    cout << "--- FTP Server pronto sulla porta " << _port << " ---" << endl;

    while (true) {
        int client_sock = accept(_server_fd, nullptr, nullptr);
        if (client_sock >= 0) {
            // Creiamo un thread e lo stacchiamo (.detach) per gestire il client
            thread(&Server::handleClient, this, client_sock).detach();
        }
    }
}

void Server::handleClient(int client_socket) {
    /*  buffer serve per salvare il nome del file,
     *  mentre bytes indica la dimensione del nome del file
     */
    char buffer[1024] = {0};
    int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    //  Controllo che il server abbia ricevuto correttamente il nome del file
    if (bytes > 0) {
        string filename(buffer);
        cout << "Richiesto file: " << filename << endl;
        sendFile(client_socket, filename);
    }

    //  Dopo eseguito il metodo sendFile chiudiamo il socket del client
    close(client_socket);
}

void Server::sendFile(int client_socket, const string& filename) {
    namespace fs = filesystem;

    //  Controllo se il file richiesto esiste
    if (!fs::exists(filename)) {
        string msg = "FILE_NOT_FOUND";
        send(client_socket, msg.c_str(), msg.size(), 0);
        return;
    }

    /* Utilizzo il tipo di dato uint64_t per salvare interi molto grandi.
     * Serve nel caso in cui il file richiesto abbia una dimensione superiore a 4gb
     */
    uint64_t fileSize = fs::file_size(filename);
    send(client_socket, &fileSize, sizeof(fileSize), 0);

    //  Leggo in binario il file e lo copio nell buffer da passare al client
    ifstream file(filename, ios::binary);
    vector<char> file_buffer(4096); // Buffer di 4KB per efficienza

    //  Invio il file fin quando non ci sono piu' elementi binari da copiare nel buffer
    while (file.read(file_buffer.data(), file_buffer.size()) || file.gcount() > 0) {
        send(client_socket, file_buffer.data(), file.gcount(), 0);
    }
    cout << "File inviato con successo." << endl;
}