#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/FLIP.h"

#define BUFFER_SIZE 1024

void Usage (int argc, char** argv){
    printf ("Usage: %s <Server IP> <Server PORT>\n", argv[0]);
    printf ("Example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main (int argc, char** argv){

    //printf ("0\n");

    if (argc < 3){
        Usage(argc, argv);
    }

    //printf("passou 1\n");

    // Criando estrutura do socket addr
    char IP_ADDR[13];
    strcpy(IP_ADDR, argv[1]);
    char PORT[10];
    strcpy(PORT, argv[2]);

    //printf ("Passou 2\n");

    struct sockaddr_storage storage;
    if (0 != AddrParse(IP_ADDR, PORT, &storage)){
        Usage(argc, argv);
    };

    // Criação do socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (1 == s){
        LogExit("Socket");
    }

    //printf("Passou 3\n");

    // Tentando conexão
    struct sockaddr *addr = (struct sockaddr*) &storage;
    
    if(0 != connect(s, addr, sizeof(storage))){
        LogExit("Connect");
    }

    char AddrStr[BUFFER_SIZE];
    Addr2Str(addr, AddrStr, BUFFER_SIZE);
    //printf("Connected to: %s\n", AddrStr);
    printf("Conectado ao servidor.\n");

    // Comunicação
    char Buffer_Receive[BUFFER_SIZE];
    memset(Buffer_Receive, 0, BUFFER_SIZE);

    char Buffer_Send[BUFFER_SIZE];
    memset(Buffer_Send, 0, BUFFER_SIZE);

    /*printf("msg> ");
    scanf(" %s", Buffer_Send);
    
    send(s, Buffer_Send, strlen(Buffer_Send)+1, 0);*/

    /* if (count != strlen(Buffer_Send)+1){
        LogExit("First send");
    }*/

    //printf("passou 2\n");

    while(1){

        //printf("entrou no loop\n");

        recv(s, Buffer_Receive, BUFFER_SIZE, 0);

        //if (!strcmp(Buffer_Receive, "Finalização\n")) break;
        puts(Buffer_Receive);

        fgets(Buffer_Send, BUFFER_SIZE, stdin);
        send(s, Buffer_Send, strlen(Buffer_Send)+1, 0);

    }

    //printf ("Received: %u bytes\n", total);
    puts(Buffer_Receive);
    close(s);

    return 0;
}