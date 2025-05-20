#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/FLIP.h"

#define BUFFER_SIZE 1024

void Usage(int argc, char **argv)
{
    printf("Usage: %s <v4|v6> <Server PORT>\n", argv[0]);
    printf("Example: %s v4 %s\n", argv[0], argv[1]);
    exit(EXIT_FAILURE);
}

void Inicializacao_Game_Message(GameMessage *GLOBAL)
{

    GLOBAL->client_action = 0;
    GLOBAL->client_wins = 0;
    GLOBAL->result = 0;
    GLOBAL->server_action = 0;
    GLOBAL->server_wins = 0;
    GLOBAL->type = 0;
    strcpy(GLOBAL->message, " ");
}

int main(int argc, char **argv)
{
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

    if (0 != ServerSockaddrInit(IP_Type, PORT, &storage))
    {
        printf("erro aqui\n");
        Usage(argc, argv);
    };

    printf("passou 2\n");

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
    printf("Bound to %s, waiting connection\n", addrstr);

    char Buffer_Receive[BUFFER_SIZE] = "";
    memset(Buffer_Receive, 0, BUFFER_SIZE);
    char Buffer_Send[BUFFER_SIZE] = "";
    memset(Buffer_Send, 0, BUFFER_SIZE);

    // Variáveis da comunicação

    int clientChoice = 0;

    GameMessage GLOBAL;
    Inicializacao_Game_Message(&GLOBAL);
    MessageType actualAction = 0;
    MessageType nextAction = 0;

    // Parâmetros do Client
    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)&cstorage;

    // Conexão com o Client
    socklen_t tamanho_cstorage = sizeof(cstorage);
    int csock = accept(s, caddr, &tamanho_cstorage);

    if (-1 == csock)
    {
        LogExit("Accept");
    }

    char caddrstr[BUFFER_SIZE];
    Addr2Str(addr, caddrstr, BUFFER_SIZE);
    printf("[log] Connection from %s\n", caddrstr);

    int final = 0; // Define o final da conexão

    while (1)
    {

        actualAction = nextAction;

        switch (actualAction)
        {

        case 0: // MSG_REQUEST

            printf("entrou no switch case 0\n");

            // O jogo continua/começa
            strcpy(GLOBAL.message, "Qual a sua jogada?\n");

            send(csock, GLOBAL.message, strlen(GLOBAL.message), 0);

            nextAction = MSG_RESPONSE;

            break;

        case 1: // MSG_RESPONSE

            printf("Entrou no case 1\n");

            nextAction = MSG_RESULT; // Vai para a página de resultado

            recv (csock, GLOBAL.message, MSG_SIZE, 0);
            clientChoice = atoi(GLOBAL.message);

            // Jogada recebida
            switch (clientChoice)
            {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
                int resultado = PlayProcessor(clientChoice);
                GLOBAL.result = resultado;

                if (resultado == 1)
                {
                    // Vitória
                    GLOBAL.client_wins += 1;
                    strcpy(GLOBAL.message, "Resultado da partida: Vitória\n");
                }
                else if (resultado == 0)
                {
                    strcpy(GLOBAL.message, "Resultado da partida: Empate\n");
                }
                else if (resultado == -1)
                {
                    GLOBAL.server_wins += 1;
                    strcpy(GLOBAL.message, "Resultado da partida: Derrota\n");
                }
                
                send(csock, GLOBAL.message, strlen(GLOBAL.message), 0);
                
                break;

            default:
                // Erro na escolha da jogada -> volta pra case 1
                nextAction = MSG_ERROR;
                break;
            }

        case 2: // MSG_RESULT

            if (GLOBAL.result == 0)
                nextAction = MSG_REQUEST;

            else
                nextAction = 3;
            break;

        case 3: // MSG_PLAY_AGAIN_REQUEST

            // Requisição para continuar as jogadas
            strcpy(GLOBAL.message, "Deseja continuar a jogo?\n");

            send(csock, GLOBAL.message, strlen(GLOBAL.message), 0);

            recv(csock, GLOBAL.message, MSG_SIZE, 0);

            clientChoice = atoi(GLOBAL.message);

            if (clientChoice == 1) nextAction = MSG_REQUEST;
            else if (clientChoice == 0) nextAction = MSG_END;

            break;

        case 5: // MSG_ERROR
            strcpy(GLOBAL.message, "Erro na escolha\n");
            nextAction = 0;

            break;

        case 6: // MSG_END
            strcpy(GLOBAL.message, "Finalização\n");

            final = 1;

            break;

        default:
            LogExit("Action Processor");
            break;
        }

        if (final) break; // Finaliza a comunicação com o socket
    }

    close(csock);
    close(s);

    return 0;
}