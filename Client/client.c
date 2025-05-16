#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "../include/FLIP.h"

#define BUFFER_SIZE 1024

void Usage (int argc, char** argv){
    printf ("Usage: %s <Server IP> <Server PORT>", argv[0]);
    printf ("Example: %s 127.0.0.1 51511");
    exit(EXIT_FAILURE);
}

void LogExit (const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int main (int argc, char** argv){

    if (argc < 3){
        Usage(argc, argv);
    }

    // Criando estrutura do socket addr
    char IP_ADDR[] = argv[1];
    char PORT[] = argv[2];

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

    // Tentando conexão
    struct sockaddr *addr = (struct sockaddr*) &storage;
    
    if(0 != connect(s, addr, sizeof(storage))){
        LogExit("Connect");
    }

    char AddrStr[BUFFER_SIZE];
    Addr2String(addr, AddrStr, BUFFER_SIZE);
    printf("Connected to: %s", AddrStr);

    // Comunicação
    char Buffer_Receive[BUFFER_SIZE];
    memset(Buffer_Receive, 0, BUFFER_SIZE);

    char Buffer_Send[BUFFER_SIZE];
    memset(Buffer_Send, 0, BUFFER_SIZE);

    printf("msg> ");
    fgets(Buffer_Send, BUFFER_SIZE-1, stdin);

    int count;
    count = send(s, Buffer_Send, strlen(Buffer_Send)+1, 0);

    if (count != strlen(Buffer_Send)+1){
        LogExit("First send");
    }

    memset(Buffer_Send, 0, BUFFER_SIZE); //Limpando o buffer

    unsigned total = 0; // Para garantir que a mensagem seja recebida por completo

    while(1){

        count = recv(s, Buffer_Receive + total, BUFFER_SIZE - total, 0);
        if (count == 0){
            // Fim da Conexão
            break;
        }

        total += count;
    }

    printf ("Received: %u bytes", total);
    puts(Buffer_Receive);
    close(s);

    return 0;
}