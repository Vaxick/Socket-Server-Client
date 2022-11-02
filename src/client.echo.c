/**
 * @file client.echo.c
 * @author Andrei Ursu
 * @brief File C per simulare un client echo con i socket
 * @version Latest
 * @date 2022-10-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <errno.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

/**
 * @brief Defines
 * 
 * @param SOCKET_ERROR - error code for socket creation
 * @param END_READ - char to exit the program
 * @param MAX_PROMPT_LENGTH - max length of the prompt
 */
#define SOCKET_ERROR ((int)-1)
#define END_RECV ((int)27)
#define MAX_PROMPT_LENGTH ((int)256)

/**
 * @brief simulates the prompt of a telnet client using sockets as
 * input parameters.
 * 
 * @param fd socket file descriptor integer used to receive and send data
 */
void telnetClient(int fd){
    /**
     * @brief Main variables used in the function
     * 
     * @param prompt string used to store the input from the user
     * @param promted single character used to send the byte to the server as a pointer
     * @param recvChar single character used to receive the byte from the server as a pointer
     */
    char *prompt = malloc(MAX_PROMPT_LENGTH);
    char prompted, recvChar;

    /**
     * @brief Main loop used to send and receive data from the server, the loop ends when the client recieves the END_RECV character
     */
    do{
        printf("Send msg: (esc) to end transmission\n");
        //scanf("%s", prompt);    //WARNING: scanf doesn't read the newline character fgets it's preferred
        fgets(prompt, MAX_PROMPT_LENGTH, stdin);
    
        for (int i = 0; i < strlen(prompt) ; i++) { //Loop for every character in prompt 
            prompted = prompt[i];   //Get the character from the prompt
            write(fd, &prompted, 1);    //Send the character to the server

            /**
             * @brief The client will read the character from the server and print it to the screen, if the client receives the END_RECV character it will exit the loop
             * 
             * @exception if the read function returns -1 the program will exit with an error
             */
            if (read(fd, &recvChar, sizeof(recvChar)) != SOCKET_ERROR) {
                if(recvChar != END_RECV){   
                    if(isprint(recvChar)){  //Check if the character is printable
                        printf("RECV:[%c][%d] Echoing..\n", recvChar, (unsigned int)recvChar);
                    }
                    else printf("RECV:[%d][%d] Echoing..\n", (unsigned int)recvChar, (unsigned int)recvChar);
                    fflush(stdout); 
                }
                else{   //If the character is END_RECV the program will exit the loop
                    printf("RECV:[%d][%d] Closing connection..\n", (unsigned int)recvChar, (unsigned int)recvChar);
                    fflush(stdout);
                    return;
                }
            }
            else {
                perror("read");
                exit(EXIT_FAILURE);
            }
        }
    }while((int)recvChar != END_RECV);
    free(prompt);   //Free the memory allocated for the prompt
}

int main(int argc, char *argv[]) {
    /**
     * @brief Check if the user has inserted the correct number of parameters
     * @param argc number of parameters
     * 
     * @exception if the number of parameters is not correct the program will exit with an error
     */
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <server ip> <server port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /**
     * @brief Variables used to manage the connection with the server via socket
     * 
     * @param serverAddress string used to store the server address
     * @param serverPort integer used to store the server port
     * @param clientConf struct used to store the client configuration
     * @param serverConf struct used to store the server configuration
     * @param socketClient integer used to store the socket file descriptor
     */
    char *serverAddress = argv[1];
    int serverPort = atoi(argv[2]);
    char buffer[MAX_PROMPT_LENGTH];
    struct sockaddr_in clientConf, serverConf;
    int socketClient;

    /**
     * @brief Initialize the socket
     * 
     * @param AF_INET IPv4
     * @param SOCK_STREAM TCP
     * @param 0 default protocol
     * 
     * @exception if the socket is not initialized correctly, the program will exit
     */
    if((socketClient = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief Set client configuration
     */
    clientConf.sin_family = AF_INET;
    clientConf.sin_addr.s_addr = htonl(INADDR_ANY);
    clientConf.sin_port = htons(0);

    /**
     * @brief Set server configuration
     */
    serverConf.sin_family = AF_INET;
    serverConf.sin_addr.s_addr = inet_addr(serverAddress);
    serverConf.sin_port = htons(serverPort);

    /**
     * @brief Bind the socket to the client configuration
     * 
     * @exception if the bind fails, the program will tell the user the error occurred and exit.
     */
    if (bind(socketClient, (struct sockaddr *)&clientConf, sizeof(clientConf)) == SOCKET_ERROR) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief Connect to the server, and if the connection succeeds will print the message
     * 
     * @exception if the connection fails, the program will tell the user the error occurred and exit.
     */    
    if (connect(socketClient, (struct sockaddr *)&serverConf, sizeof(serverConf)) == SOCKET_ERROR) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    else printf("Connection established with the server [%s:%d]\n", serverAddress, serverPort);

    /**
     * @brief Read the welcome message sent by the server
     * 
     * @exception if the read fails, the program will tell the user the error occurred and exit.
     */
    if (read(socketClient, buffer, sizeof(buffer)) != SOCKET_ERROR) {
        printf("Welcome message from server:\n%s\n", buffer);
        fflush(stdout); // ** force stdout to write buffer
    }
    else {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief Simulate the telnet client
     */
    telnetClient(socketClient);
    
    /**
     * @brief Read the goodbye message sent by the server
     * 
     * @exception if the read fails, the program will tell the user the error occurred and exit.
     */
    if (read(socketClient, buffer, sizeof(buffer)) != SOCKET_ERROR) {
        printf("Goodbye message from server:\n%s\n", buffer);
        fflush(stdout);
    }
    else {
        perror("read");
        exit(EXIT_FAILURE);
    }

    /**
     * @brief Close the socket
     */
    printf("\nsocket::close\n");
    close(socketClient);

    return EXIT_SUCCESS;
}
