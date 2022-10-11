/**
 * @file server.echo.c
 * @author Andrei Ursu e Alessio Mucchi
 * @brief File C per simulare un server echo con i socket
 * @version Latest
 * @date 2022-10-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

/**
 * @brief Definizione delle seguenti costanti:
 * @param SOCKET_ERROR - codice di errore per la creazione del socket
 * @param SOCKET_PORT - porta su cui il server ascolta
 * @param RETURN - codice ascii per il carattere di ritorno a capo
 * @param QUIT - codice ascii per il carattere di uscita
 */
#define SOCKET_ERROR    ((int)-1)
#define SOCKET_PORT ((int)5000)
#define RETURN  ((int)13)
#define QUIT    ((int)27)

int main(unsigned argc, char *argv[]){

    // Definizione dei socket
    int handshakingSocket, commSocket;

    int readASCII = 0;
    int clientSize = 0;
    int asciiVal = 0;
    
    //Struct per la gestione dei socket
    struct sockaddr_in server, client;

    //Do i parametri alla struct server per il bind del socket
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SOCKET_PORT);

    //Creo il socket server di handshaking
    if ((handshakingSocket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR){
        //Managment of the error
        perror("handshakingSocket");
        return 1;
    }
    //"lego" il server socket alle specifiche del server immesse nella struct
    if (bind(handshakingSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR){
        //Managment of the error
        perror("bind");
        return 1;
    }

    /**
     * @details Metto il server socket in ascolto
     * TODO: Scoprire cosa fa il secondo parametro di listen
     */
    if (listen(handshakingSocket, 1) == SOCKET_ERROR){
        //Managment of the error
        perror("listen");
        return 1;
    }
    /**
     * @details: Messaggio in console per indicare che il server è pronto ad accettare connessioni
     */
    printf("Echo Server: ready on port [%d]\n", SOCKET_PORT);

    /**
     * @details Inserisco in clientSize la dimensione della struct client per utilizzarla come parametro
     * @param clientSize
     */
    clientSize = sizeof(client);

    /**
     * @details Accetto la connessione del client sul server
     */
    if ((commSocket = accept(handshakingSocket, (struct sockaddr *)&client, &clientSize)) == SOCKET_ERROR){
        //Managment of the error
        perror("accept");
        return 1;
    }

    /**
     * @details Avviso il client che la connessione è stata accettata
     */
    send( commSocket , "Hello! Echo server here.\nTo end connection type and enter esc\n", 63, 0);

    /**
     * @brief Ciclo do/while per leggere i caratteri inviati dal client, mostrarli in console e reinviarli al client, come
     * un echo per far capire al client che il server ha ricevuto i dati, tutto ciò finchè il client non invia il carattere
     * QUIT, che equivale a esc, in questo caso il server chiude la connessione.
     */
    do {
        if (recv(commSocket, &readASCII, 1, 0) != SOCKET_ERROR) {
            asciiVal = (int)readASCII;
            if (asciiVal != RETURN && asciiVal != QUIT){
                //Messaggio dei dati ricevuti dal client
                if(isprint(asciiVal)){
                    printf("RECV:[%c][%d] Echoing..\n", readASCII, asciiVal);
                }
                else printf("RECV:[%d][%d] Echoing..\n", readASCII, asciiVal);
                //Echo dei dati ricevuti dal client al client
                send( commSocket , &readASCII, 1, 0);
            }
        }
        else{
            //Managment of the error
            perror("recv");
            return 1;
        }
    } while (readASCII != QUIT);
    send( commSocket , "Bye!\nClosing connection...\n", 30, 0);
    //Chiudo la connessione con il client
    close(commSocket);
    fprintf(stderr, "Server connection closed.\n");
}
