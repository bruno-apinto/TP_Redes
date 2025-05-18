#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/FLIP.h"

#define BUFFER_SIZE 1024

void Usage (int argc, char** argv){
    printf ("Usage: %s <v4|v6> <Server PORT>\n", argv[0]);
    printf ("Example: %s v4 %s\n", argv[0], argv[1]);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv){
    /*if (argc < 3){
        Usage(argc, argv);
    }*/

    printf("passou 1\n");
    // Inicializando a interface
    char IP_Type[4];
    char PORT[10];
    strcpy(IP_Type, argv[1]);
    strcpy(PORT, argv[2]);

    struct sockaddr_storage storage;

        if (0 != ServerSockaddrInit(IP_Type, PORT, &storage)){
            printf("erro aqui\n");
            Usage(argc, argv);
        };
        
    printf("passou 2\n");

     // Criação do socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (1 == s){
        LogExit("Socket");
    }

    // Abertura Passiva
    struct sockaddr *addr = (struct sockaddr*) &storage;

    if (0 != bind(s, addr, sizeof(storage))){
        LogExit("Bind");
    }

    if (0 != listen(s, 10)){
        LogExit("Listen");
    }

    char addrstr[BUFFER_SIZE];
    Addr2Str(addr, addrstr, BUFFER_SIZE);
    printf("Bound to %s, waiting connection\n", addrstr);

    char Buffer_Receive[BUFFER_SIZE] = "";
    memset(Buffer_Receive, 0, BUFFER_SIZE);
    char Buffer_Send[BUFFER_SIZE] = "";
    memset(Buffer_Send, 0, BUFFER_SIZE);
    
    while(1){

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr*) &cstorage;

        socklen_t tamanho_cstorage = sizeof(cstorage);

        int csock = accept(s, caddr, &tamanho_cstorage);

        if (-1 == csock){
            LogExit("Accept");
        }

        char caddrstr[BUFFER_SIZE];
        Addr2Str(addr, caddrstr, BUFFER_SIZE);
        printf("[log] Connection from %s\n", caddrstr);

        //Recebendo mensagem
        size_t count = recv(csock, Buffer_Receive, BUFFER_SIZE, 0);
        printf("[MSG] %s, %d bytes: %i", caddrstr, (int) count, BUFFER_SIZE);
    
        //Enviando mensagem
        sprintf(Buffer_Send, "Remote endpoint: %.1000s\n", caddrstr);
        count = send(csock, Buffer_Send, strlen(Buffer_Send)+1, 0);
        if (count != strlen(Buffer_Send)){
            LogExit("Send");
        }
        
        close(csock);
    }

    close(s);

    return 0;
}