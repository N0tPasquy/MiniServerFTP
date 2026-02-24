// TIP Progetto di Reti di Calcolatori di Pagano Pasquale MAT: 0124003182

#include <iostream>
#include <thread>
#include <chrono>
#include "Server.hpp"
#include "Client.hpp"

using namespace std;

int main() {
    try {
        // Avvio il Server in un thread separato
        thread serverThread([]() {
            try {
                Server server(8080);
                server.run(); // Questo loop è bloccante
            } catch (const exception& e) {
                cerr << "Errore Server: " << e.what() << endl;
            }
        });

        // Aspetto un secondo per dare tempo al server di aprirsi
        this_thread::sleep_for(chrono::seconds(1));

        // Avvio la logica del Client nel thread principale
        cout << "--- Client: Avvio richiesta download ---" << endl;
        Client client("127.0.0.1", 8080);

        // Proviamo a scaricare un file di testo "test.txt"
        string fileToRequest = "test.txt";

        if (client.downloadFile(fileToRequest)) { //
            cout << "--- Client: Download completato con successo! ---" << endl;
        } else {
            cout << "--- Client: Download fallito. ---" << endl;
        }

        // In questo scenario, dopo il download il main finisce.
        // Poiché il serverThread è infinito, lo "stacchiamo" o chiudiamo il programma.
        cout << "Chiusura in corso..." << endl;

        // Terminiamo forzatamente
        exit(0);

    } catch (const exception& e) {
        cerr << "Errore nel Main: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}