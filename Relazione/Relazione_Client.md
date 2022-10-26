# **Relazione tecnica - Socket Client**

> ## **Indice**
>1) [**Definizione del problema**](#1-definizione-del-problema)
>2) [**Architettura**](#2-architettura)
>3) [**Criticità da discutere**](#3-criticità-da-discutere)
>4) [**Codice**](#4-codice)
>5) [**Testing**](#5-testing)
>6) [**Conclusioni**](#6-conclusioni)

## *1) Definizione del problema*
Lo scopo di questa esercitazione è quello di creare un client echo utilizzando i socket, bisogna quindi potersi connettersi al server tramite il client, e inviare dei caratteri per poi riceverli di nuovo come ricevuta conferma.

## *2) Architettura*
![FlowChart](flowchart.png)

## *3) Criticità da discutere*

### *3.1) Criticità: Codice sviante*
___
Il codice in certi punti è stato sviante, quindi voleva fare cose fondalmentalmente sbagliate, come ad esempio utilizzare 2 socket, uno del client e uno del server, invece di utilizzare esclusivamente il socket del client.

Un'altro caso è stato un fake FIXME alla conversione da stringa a intero tramite un atoi

Definita una variabile SOCKET_PORT, che non è corretta siccome la porta la si inserisce al momento dell'esecuzione del programma.

## *4) Codice*
In questo esercizio ho utilizzato la libreria `sys/socket.h` per la gestione dei socket e la libreria `netinet/in.h` per la famiglia dei socket.

Define utilizzati nel codice:
```c
#define SOCKET_ERROR ((int)-1)
#define END_RECV ((int)-27)
#define MAX_PROMPT_LENGTH ((int)256)
```
___

All'interno del main:

Controlliamo che l'utente abbia inserito il corretto numero di parametri associato ad argv

In caso contrario il programma avvisa l'utente e quitta:
```c
if (argc < 3) {
    fprintf(stderr, "Usage: %s <server ip> <server port>\n", argv[0]);
    exit(EXIT_FAILURE);
}
```

___
Impostiamo i parametri per permettere la connessione col server tramite il socket:
```c
char *serverAddress = argv[1];
int serverPort = atoi(argv[2]);
char buffer[MAX_PROMPT_LENGTH];
struct sockaddr_in clientConf, serverConf;
int socketClient;
```

___
Inizializiamo il socket client:
```c
 if((socketClient = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR) {
    perror("socket");
    exit(EXIT_FAILURE);
}

```

___

Definiamo la configurazione per il client e per il server:
```c
clientConf.sin_family = AF_INET;
clientConf.sin_addr.s_addr = htonl(INADDR_ANY);
clientConf.sin_port = htons(0);

serverConf.sin_family = AF_INET;
serverConf.sin_addr.s_addr = inet_addr(serverAddress);
serverConf.sin_port = htons(serverPort);
```

___


`Leghiamo` le configuarioni del client al socket client

Se la bind fallisce il programma quitta:
```c
if (bind(socketClient, (struct sockaddr *)&clientConf, sizeof(clientConf)) == SOCKET_ERROR) {
    perror("bind");
    exit(EXIT_FAILURE);
}
```

___

Colleghiamo il socket client al server, in caso di successo stamperà un messaggio:
```c
if (connect(socketClient, (struct sockaddr *)&serverConf, sizeof(serverConf)) == SOCKET_ERROR) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    else printf("Connection established with the server [%s:%d]\n", serverAddress, serverPort);

```

___

Leggiamo il messaggio di benvenuto inviato dal server, se la lettura fallisce il programma avviserà l'utente e quitterà:
```c
 if (read(socketClient, buffer, sizeof(buffer)) != SOCKET_ERROR) {
        printf("Welcome message from server:\n%s\n", buffer);
        fflush(stdout); // ** force stdout to write buffer
    }
    else {
        perror("read");
        exit(EXIT_FAILURE);
    }
```

___

Simula il telnet del client:
```c
telnetClient(socketClient);
```

___

Viene letto il messaggio di termine inviato dal server, se fallisce la lettura il programma avviserà l'utente e quitterà:
```c
 if (read(socketClient, buffer, sizeof(buffer)) != SOCKET_ERROR) {
        printf("Goodbye message from server:\n%s\n", buffer);
        fflush(stdout);
    }
    else {
        perror("read");
        exit(EXIT_FAILURE);
    }
```

___

Chiudiamo il socket:
```c
 printf("\nsocket::close\n");
    close(socketClient);

    return EXIT_SUCCESS;
```

___

All'interno del TelnetClient:

Creo le variabili che mi serviranno nella funzione:
```c
char *prompt = malloc(MAX_PROMPT_LENGTH);
char prompted, recvChar;
```

___

All'interno di un ciclo do_while viene svolto il grosso del lavoro, ovvero tramite un loop continua a ricevere i dati dal server e aspettare, terminando solo quando il client invia il carattere `END_RECV`
```c
do{
    printf("Send msg: (esc) to end transmission\n");
    fgets(prompt, MAX_PROMPT_LENGTH, stdin);
    
    for (int i = 0; i < strlen(prompt) ; i++) { 
        prompted = prompt[i];   
        write(fd, &prompted, 1);  

        if (read(fd, &recvChar, sizeof(recvChar)) != SOCKET_ERROR) {
            if(recvChar != END_RECV){   
                if(isprint(recvChar)){ 
                    printf("RECV:[%c][%d] Echoing..\n", recvChar, (unsigned int)recvChar);
                }
                else printf("RECV:[%d][%d] Echoing..\n", (unsigned int)recvChar, (unsigned int)recvChar);
                fflush(stdout); 
            }
            else{
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
free(prompt);

