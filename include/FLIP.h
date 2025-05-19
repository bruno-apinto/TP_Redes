#ifndef FLIP_H
#define FLIP_H

/**
 * @brief Realiza a transferêrencia de informações interface para o struct sockaddr*, parametrizando a versão IP
 *
 * @param addrstr String do endereço
 * @param portstr String da porta
 * @param storage Interface das informações
 * @return int sucesso da operação
 */
int AddrParse(const char *addrstr, const char *portstr,
              struct sockaddr_storage *storage);

/**
 * @brief Converte o endereço para o formato string
 *
 * @param addr endereço sock
 * @param str
 * @param strsize tamanho da string a ser copiada
 */
void Addr2Str(const struct sockaddr *addr, char *str, size_t strsize);

/**
 * @brief Inicializador do servidor
 *
 * @param proto Protocolo utilizado, IPv4 ou Ipv6
 * @param portstr POrta utilizada
 * @param storage Interface utilizada
 * @return int sucesso da operação
 */
int ServerSockaddrInit(const char *proto, const char *portstr,
                       struct sockaddr_storage *storage);

/**
 * @brief Mensagem de erro no log
 *
 * @param msg
 */
void LogExit(const char *msg);

#define MSG_SIZE 256

/**
  * @brief Tipo da mensagem
 */
typedef enum{
    MSG_REQUEST,
    MSG_RESPONSE,
    MSG_RESULT,
    MSG_PLAY_AGAIN_REQUEST,
    MSG_PLAY_AGAIN_RESPONSE,
    MSG_ERROR,
    MSG_END
} MessageType;

/**
 * @brief Estrutura das mensagens trocas. Tipo (int), Ação do cliente (int),
 * @brief Ação do servidor (int), Resultado da jogada (int), Ganhos do clinte (int),
 * @brief Ganhos do servidor (int), mensagem (char[MSG_SIZE]).
 */
typedef struct{
    int type; // Tipo da mensagem
    int client_action;
    int server_action;
    int result;
    int client_wins;
    int server_wins;
    char message[MSG_SIZE];
} GameMessage;

int ActionProcessor(int clientChoice, 
    MessageType actualAction, GameMessage *GLOBAL);

int PlayProcessor (int clientChoice);

#endif