#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "../include/FLIP.h"

#define BUFFER_SIZE MSG_SIZE

char bufferMultiplicador[BUFFER_SIZE] = "";
float multiplicador = 0.00;

void Usage(int argc, char **argv)
{
    printf("Usage: %s <v4|v6> <Server PORT>\n", argv[0]);
    printf("Example: %s v4 %s\n", argv[0], argv[1]);
    exit(EXIT_FAILURE);
}

int MakingConnection(int server_socket){

     // Parâmetros do Client
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)&cstorage;

    // Conexão com o Client
    socklen_t tamanho_cstorage = sizeof(cstorage);
    int client_socket = accept(server_socket, caddr, &tamanho_cstorage);

    if (-1 == client_socket){
        LogExit("Accept");
    }

    return client_socket;
}

void Broadcast(int *client_sockets, int *sockets_in_use){

    for (int i = 0; i<=10; i++){
        if (i == 10) i = 0;

        if (sockets_in_use[i] == 1){
            send(client_sockets[i], "1", strlen("1") + 1, 0);
            send(client_sockets[i], bufferMultiplicador, strlen(bufferMultiplicador) + 1, 0);
        }

        if (multiplicador == 2.00) {

            i = 0;
            while (i<10){
                //estourou
                i++;
            }
            
            break;
        }
    }

}

int main(int argc, char **argv)
{
    /*if (argc < 3){
        Usage(argc, argv);
    }*/

    //printf("passou 1\n");
    // Inicializando a interface
    char IP_Type[4];
    char PORT[10];
    strcpy(IP_Type, argv[1]);
    strcpy(PORT, argv[2]);

    struct sockaddr_storage storage;

    if (0 != ServerSockaddrInit(IP_Type, PORT, &storage))
    {
        //printf("erro aqui\n");
        Usage(argc, argv);
    };

    //printf("passou 2\n");

    // Criação do socket
    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (1 == s)
    {
        LogExit("Socket");
    }

    // Abertura Passiva
    struct sockaddr *addr = (struct sockaddr *)&storage;

    if (0 != bind(s, addr, sizeof(storage)))
    {
        LogExit("Bind");
    }

    if (0 != listen(s, 10))
    {
        LogExit("Listen");
    }

    char addrstr[BUFFER_SIZE];
    Addr2Str(addr, addrstr, BUFFER_SIZE);

    printf("Servidor iniciado em modo IP%s na porta %s. Aguardando conexão...\n", IP_Type, PORT);
    //printf("Bound to %s, waiting connection\n", addrstr);

    // Loop do servidor

    while(1){

            int csock_available[10] = {0 ,0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            int csock[10];

            // thread 1
            csock[0] = MakingConnection(s); // Estabelece conexão com um cliente

            // thread 2
            Broadcast(&csock_available, &csock);

            char caddrstr[BUFFER_SIZE];
            Addr2Str(addr, caddrstr, BUFFER_SIZE);
            printf("Cliente conectado.\n");
            //printf("[log] Connection from %s\n", caddrstr);

            // Encerrando conexão
            printf("Encerrando conexão.\n");
            close(csock);
            printf("Cliente desconectado.\n");
    }

    close(s);

    return 0;
}