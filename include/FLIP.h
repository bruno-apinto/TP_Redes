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
int ServerSockaddrInit (const char *proto, const char *portstr, 
    struct sockaddr_storage *storage);

    /**
     * @brief Mensagem de erro no log
     * 
     * @param msg 
     */
void LogExit (const char* msg);
#endif