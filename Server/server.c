#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/FLIP.h"

#define BUFFER_SIZE MSG_SIZE

char ATTACK_TYPES[5][17] = {"Nuclear Attack", "Intercept Attack", "Cyber Attack", "Drone Attack", "Bio Attack"};

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
    strcpy(GLOBAL->message, "\0");
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

    char Buffer_Receive[BUFFER_SIZE] = "";
    memset(Buffer_Receive, 0, BUFFER_SIZE);
    char Buffer_Send[BUFFER_SIZE] = "";
    memset(Buffer_Send, 0, BUFFER_SIZE);

    // Loop do servidor

    while(1){ 

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
            printf("Cliente conectado.\n");
            //printf("[log] Connection from %s\n", caddrstr);

            int final = 0; // Define o final da conexão
            MessageType origem_error = MSG_REQUEST;

            while (1)
            {
                actualAction = nextAction;

                switch (actualAction)
                {

                case 0: // MSG_REQUEST

                    //printf("MSG_REQUEST\n");
                    printf("Apresentando as opções para o cliente.\n");

                    // O jogo continua/começa
                    strcat(GLOBAL.message, "\nEscolha a sua jogada:\n\n0 - Nuclear Attack\n1 - Intercept Attack\n2 - Cyber Attack\n3 - Drone Strike\n4 - Bio Attack\n\n");
                    send(csock, GLOBAL.message, strlen(GLOBAL.message)+1, 0);

                    //printf("MSG_RESPONSE\n");

                    recv (csock, Buffer_Receive, MSG_SIZE-1, 0);
                    clientChoice = atoi(Buffer_Receive);

                    printf("Cliente escolheu %d.\n", clientChoice);
                    //printf ("Cliente escoheu: %d\n", clientChoice);

                    // Jogada recebida
                    switch (clientChoice)
                    {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                        int serverChoice = rand() % 4;
                        printf("Servidor escolheu aleatoriamente %d.\n", serverChoice);
                        int resultado = PlayProcessor(clientChoice, serverChoice);
                        GLOBAL.result = resultado;

                        if (resultado == 1)
                        {
                            // Vitória
                            GLOBAL.client_wins += 1;
                            //strcpy(GLOBAL.message, "Resultado: Vitória!\n");
                            sprintf(GLOBAL.message, "\nVocê escolheu: %s\nServidor escolheu: %s\nResultado: Vitória!\n\n", 
                                                                    ATTACK_TYPES[clientChoice], ATTACK_TYPES[serverChoice]);
                        }
                        else if (resultado == -1)
                        {
                            //strcpy(GLOBAL.message, "Resultado: Empate!\n");
                            sprintf(GLOBAL.message, "\nVocê escolheu: %s\nServidor escolheu: %s\nResultado: Empate!\n\n", 
                                                                    ATTACK_TYPES[clientChoice], ATTACK_TYPES[serverChoice]);
                        }
                        else if (resultado == 0)
                        {
                            GLOBAL.server_wins += 1;
                            //strcpy(GLOBAL.message, "Resultado: Derrota!\n");
                            sprintf(GLOBAL.message, "\nVocê escolheu: %s\nServidor escolheu: %s\nResultado: Derrota!\n\n", 
                                                                    ATTACK_TYPES[clientChoice], ATTACK_TYPES[serverChoice]);
                        }
                        else {
                            origem_error = MSG_REQUEST;
                            strcpy(GLOBAL.message, "Erro ao processar partida\n");
                        }

                        nextAction = MSG_RESULT;
                        
                        break;

                    default:
                        // Erro na escolha da jogada -> volta pra case 1
                        origem_error = MSG_RESPONSE;
                        nextAction = MSG_ERROR;
                        break;
                    }
                    break;

                case 2: // MSG_RESULT

                    //printf("MSG_RESULT\n");
                    //send(csock, GLOBAL.message, strlen(GLOBAL.message)+1, 0);

                    if (GLOBAL.result == -1){
                        //recv(csock, Buffer_Receive, BUFFER_SIZE+1, 0);
                        nextAction = MSG_REQUEST;
                    }

                    else {
                        nextAction = MSG_PLAY_AGAIN_REQUEST;
                        //recv(csock, Buffer_Receive, BUFFER_SIZE+1, 0);
                    }
                    break;

                case 3: // MSG_PLAY_AGAIN_REQUEST

                    //printf("MSG_PLAY_AGAIN_REQUEST\n");

                    printf("Perguntando se o cliente deseja jogar novamente.\n");

                    // Requisição para continuar as jogadas
                    strcat(GLOBAL.message, "Deseja jogar novamente?\n1 - Sim\n0 - Não\n");

                    send(csock, GLOBAL.message, strlen(GLOBAL.message)+1, 0);

                    recv(csock, Buffer_Receive, MSG_SIZE-1, 0);

                    clientChoice = atoi(Buffer_Receive);

                    //printf("Cliente decidiu continuar: %d\n", clientChoice);

                    if (clientChoice == 1) {
                        nextAction = MSG_REQUEST;
                        GLOBAL.message[0] = '\0';
                    }
                    else if (clientChoice == 0) {
                        nextAction = MSG_END;
                        printf("Cliente não deseja jogar novamente.\n");
                    }
                    else {
                        nextAction = MSG_ERROR;
                        origem_error = MSG_PLAY_AGAIN_RESPONSE;
                    }

                    break;

                case 5: // MSG_ERROR

                    //printf("MSG_ERROR\n");
                    //strcpy(GLOBAL.message, "\nErro na escolha\n");
                    //send(csock, GLOBAL.message, strlen(GLOBAL.message)+1, 0);

                    if (origem_error == MSG_PLAY_AGAIN_RESPONSE){
                        printf("Erro: resposta inválida para jogar novamente.\n");
                        strcpy(GLOBAL.message, "\nPor favor, digite 1 para jogar novamente ou 0 para encerrar.\n");
                        nextAction = MSG_PLAY_AGAIN_REQUEST;
                    }
                    else if (origem_error == MSG_RESPONSE){
                        printf("Erro: opção inválida de jogada.\n");
                        strcpy(GLOBAL.message, "\nPor favor, selecione um valor de 0 a 4.\n");
                        nextAction = MSG_REQUEST;
                    }
                    
                    else {
                        printf ("Erro não tratado: %d\n", origem_error);
                        nextAction = MSG_REQUEST;
                    }

                    break;

                case 6: // MSG_END

                    //printf("MSG_END\n");
                    printf("Enviando placar final.\n");
                    sprintf(GLOBAL.message, "\nFim de jogo!\nPlacar final: Você %d x %d Servidor\nObrigado por jogar!\n", GLOBAL.client_wins, GLOBAL.server_wins);
                    send(csock, GLOBAL.message, strlen(GLOBAL.message)+1, 0);

                    //sprintf(GLOBAL.message, "Placar final: Você %d x %d Servidor\nObrigado por jogar!\n", GLOBAL.client_wins, GLOBAL.server_wins);
                    //send(csock, GLOBAL.message, strlen(GLOBAL.message)+1, 0);

                    final = 1;

                    break;

                default:
                    LogExit("Action Processor");
                    break;
                }

                if (final) break; // Finaliza a comunicação com o socket
            }
            
            // Encerrando conexão
            printf("Encerrando conexão.\n");
            close(csock);
            printf("Cliente desconectado.\n");
    }

    close(s);

    return 0;
}