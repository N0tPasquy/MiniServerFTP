//
// Created by pasquale on 24/02/26.
//
#include "Client.hpp"

using namespace std;

Client::Client(const string& ip, int port)
    : _server_ip(ip), _server_port(port), _sock_fd(-1) {}
    /*  Inizializzo il descrittore _sock_fd a -1 per evitare che il server cerchi di
     *  chiudere socket "fantasma" se la connessione non viene mai avviata
    */

Client::~Client() {
    //  Distruttore uguale a quello del Server
    if (_sock_fd >= 0) close(_sock_fd);
}

bool Client::connectToServer() {
    //  Creo il socket
    _sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock_fd < 0) return false;

    //  Carta di identita' del Client
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(_server_port);

    //  Converte l'IP in leggibbile alla scheda di rete
    if (inet_pton(AF_INET, _server_ip.c_str(), &server_addr.sin_addr) <= 0) {
        return false;
    }

    //  Ora proviamo a fare la prima connessione al Server
    if (connect(_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return false;
    }
    return true;
}

bool Client::downloadFile(const string& filename) {
    //  Controllo che la connessione al server sia andata a buon fine
    if (!connectToServer()) {
        cerr << "Connessione fallita!" << endl;
        return false;
    }

    //  Invia il nome del file desiderato
    send(_sock_fd, filename.c_str(), filename.size(), 0);

    //  Riceve la dimensione del file
    uint64_t fileSize;
    int bytes_received = recv(_sock_fd, &fileSize, sizeof(fileSize), 0);

    //  Controllo se mi è stata mandata una dimensione corretta per un file
    if (bytes_received <= 0) {
        cerr << "File non trovato sul server o errore." << endl;
        return false;
    }

    /*  Creo un flusso di uscita per un file chiamato downloaded_[nome],
     *  lo tratto come un file binario e mi preparo a scriverci dentro. */
    ofstream outFile("downloaded_" + filename, ios::binary);
    vector<char> buffer(4096);
    uint64_t total_received = 0;

    /*  Ricevo i byte dal server a blocchi (tramite il buffer) e li scrivo immediatamente
        nel file locale finché non raggiungo la dimensione totale prevista. */
    while (total_received < fileSize) {
        //  r indica il numero di byte da ricevere
        int r = recv(_sock_fd, buffer.data(), buffer.size(), 0);
        if (r <= 0) break; // Connessione chiusa o errore

        outFile.write(buffer.data(), r);    //  Scrivo sul file locale
        total_received += r;    //  Aggiorno il totale di byte ricevuti
    }

    cout << "--- CLient: " << total_received << " byte ricevuti." << endl;
    outFile.close();
    return true;
}