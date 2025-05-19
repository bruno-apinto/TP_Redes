#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "include/FLIP.h"

int AddrParse(const char *addrstr, const char *portstr, 
            struct sockaddr_storage *storage){

    if (addrstr == NULL || portstr == NULL){
        return -1;
    }

    uint16_t port = (uint16_t) atoi(portstr); //unsigned short
    if (port == 0){
        return -1;
    }

    port = htons(port); // Convertendo para a versão da Rede

    // IPv4
    struct in_addr inaddr4; //32 bits
    if (inet_pton(AF_INET, addrstr, &inaddr4)){
        struct sockaddr_in *addr4 = (struct sockaddr_in*) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    // IPv6
    struct in6_addr inaddr6; //128 bits
    if (inet_pton(AF_INET6, addrstr, &inaddr6)){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6*) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        // addr6->sin6_addr = inaddr6;
        memcpy(&(addr6->sin6_addr), &(inaddr6), sizeof(inaddr6));
       return 0;
    }

    return -1;

}

void Addr2Str (const struct sockaddr *addr, char *str, size_t strsize){
    
    int version;
    char addrstr[INET6_ADDRSTRLEN+1] = "";
    uint16_t port;

    if (addr->sa_family == AF_INET){

        version = 4;
        struct sockaddr_in *addr4 = (struct sockaddr_in *) addr;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrstr, INET6_ADDRSTRLEN+1)){
            LogExit("Addr2Str/ntop4");
        }

        port = ntohs(addr4->sin_port);

    }

    else if (addr->sa_family == AF_INET6){
        version = 6;
        struct sockaddr_in *addr6 = (struct sockaddr_in *) addr;
        if (!inet_ntop(AF_INET, &(addr6->sin_addr), addrstr, INET6_ADDRSTRLEN+1)){
            LogExit("Addr2Str/ntop6");
        }

        port = ntohs(addr6->sin_port);

    }

    else{
        
        LogExit("Unknown Protocol Family");
    }
    if (str){
        snprintf(str, strsize, "IPv%d %s %hu", version, addrstr, port);
    }
}

int ServerSockaddrInit(const char *proto, const char *portstr, 
                    struct sockaddr_storage *storage){
    
    uint16_t port = (uint16_t) atoi(portstr);

    printf ("porta: %d\n", port);
    printf("protocolo: %s\n", proto);

    if (port == 0) {
        return -1;
    }
    port = htons(port);

    memset(storage, 0, sizeof(*storage));

    if (0 == strcmp(proto, "v4")){
        struct sockaddr_in *addr4 = (struct sockaddr_in*) storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY;

        return 0;
    }
    else if (0 == strcmp(proto, "v6")){
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6*) storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        
       return 0;
    }
    else{
        return -1;
    }
    return 0;
}

void LogExit (const char* msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

int ActionProcessor(int clientChoose, int actualAction, GameMessage *GLOBAL){

    switch (actualAction){

    case  0:

        // Requisição para continuar as jogadas
        strcpy("Deseja continuar a jogo?\n", GLOBAL->message);
        break;
    
    case 1:

        // O jogo continua
        strcpy("Qual a sua jogada?\n", GLOBAL->message);
        break;
    
    case 2:

        // Jogada recebida
        switch (clientChoose){
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            int resultado = PlayProcessor(clientChoose);
            GLOBAL->result = resultado;

            if (resultado == 1){
                //Vitória
                GLOBAL->client_wins += 1;
                strcpy("VITÓRIA\n", GLOBAL->message);
            }
            else if (resultado == 0){
                strcpy("Empate\n", GLOBAL->message);
            }
            else if (resultado == -1){
                GLOBAL->server_wins += 1;
                strcpy("Derrota\n", GLOBAL->message);
            }
            break;
        
        default:
         // Erro na escolha da jogada -> volta pra case 1
            break;
        }
        
    
    default:
        LogExit("Action Processor");
        break;
    }
}

int PlayProcessor (int clientChoose){

    int jogadaServidor = rand() % 4;

    if (jogadaServidor == clientChoose){
        // Empate
        return -1;
    }

    switch (clientChoose){

    case 0:
        if (jogadaServidor == 2 || jogadaServidor == 3){
            // Vitória
            return 1;
        }
        else if (jogadaServidor == 1 || jogadaServidor == 4){
            // Derrota
            return 0;
        }
        break;

    case 1:
        if (jogadaServidor == 4 || jogadaServidor == 0){
            // Vitória
            return 1;
        }
        else if (jogadaServidor == 2 || jogadaServidor == 3){
            // Derrota
            return 0;
        }
        break;

    case 2:
        if (jogadaServidor == 1 || jogadaServidor == 3){
            // Vitória
            return 1;
        }
        else if (jogadaServidor == 0 || jogadaServidor == 4){
            // Derrota
            return 0;
        }
        break;
    
    case 3:
        if (jogadaServidor == 1 || jogadaServidor == 4){
            // Vitória
            return 1;
        }
        else if (jogadaServidor == 0 || jogadaServidor == 2){
            // Derrota
            return 0;
        }
        break;
    
    case 4:
        if (jogadaServidor == 0 || jogadaServidor == 2){
            // Vitória
            return 1;
        }
        else if (jogadaServidor == 1 || jogadaServidor == 3){
            // Derrota
            return 0;
        }
        break;

    default:
        break;
    }

}
