#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

char* TrataLocal(char *instrucao[]) 
{
    if ((strstr(instrucao, "maxsensor")) == 0) {                

        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_LS");
        
        return comando;
    } else if ((strstr(instrucao, "potency")) == 0) {

        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_LP");
        
        return comando;
    }

    return NULL;
}

char* TrataExternal(char *instrucao[]) 
{
    if ((strstr(instrucao, "maxsensor")) == 0) {                

        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_ES");
        
        return comando;
    } else if ((strstr(instrucao, "potency")) == 0) {

        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_EP");
        
        return comando;
    }

    return NULL;
}

char* TrataGlobal(char *instrucao[]) 
{
    if ((strstr(instrucao, "maxsensor")) == 0) {                

        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_MS");
        
        return comando;
    } else if ((strstr(instrucao, "maxnetwork")) == 0) {

        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_MN");
        
        return comando;
    }

    return NULL;
}


int main(int argc, char **argv)
{
    struct sockaddr_storage storage;                // estrutura de armazenamento p/ ipv6 ou ipv4
     //type of socket created
    storage.ss_family = AF_INET;
    
    // criação do socket
    int s = socket(storage.ss_family, SOCK_STREAM, 0); // CRIA SOCKET CONEXÃO INTERNET COM TCP (lib types e socket)
    if (s == -1)
    {
        logExit("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); // instanciação do endereço
    if (0 != connect(s, addr, sizeof(storage)))            // CONECTA NO SOCKET
    {
        logExit("connect");
    }

    char addrstr[BUFSZ];
    // Configurar o socket 's' e conectar ao servidor

    int selected = 0;

     while (1) {
    char buf[BUFSZ];
    char string[BUFSZ]; // Variável para armazenar a string recebida
    char comando[BUFSZ]; // Variável para armazenar a comando recebida

    memset(buf, 0, BUFSZ);

    // Recebe a instrução do terminal
    fgets(buf, sizeof(buf), stdin);
    strncpy(string, buf, sizeof(string) - 1);

    bzero(buf, strlen(buf)); // Limpa o buffer

    // Remova a quebra de linha do final da string
    string[strcspn(string, "\n")] = '\0';

    // Processa a instrução e cria um array de elementos
    char *instrucao[BUFSZ];
    char *mensagem[BUFSZ];
    int numTokens = 0;

    // Trata a instrução
    if ((strstr(instrucao, "external")) == 0) {  

        char *comando =  instalarSensor();
        if (comando != NULL) {
            send(s, comando, strlen(comando), 0);
            free(comando);
        }
    } else if ((strstr(instrucao, "local")) == 0) {                

        char *comando = TrataLocal(instrucao);
        if (comando != NULL) {
            send(s, comando, strlen(comando), 0);
            free(comando);
        }
    } else if ((strstr(instrucao, "global")) == 0) {                

        char *comando = TrataGlobal(instrucao);
        if (comando != NULL) {
            send(s, comando, strlen(comando), 0);
            free(comando);
        }
    } else if (strcmp(comando, "kill") == 0) {
        close(s);
        printf("Servidor encerrado pelo cliente.\n");
        exit(EXIT_SUCCESS);
    } 

    bzero(buf, strlen(buf));

    // Recebe a mensagem do servidor
    ssize_t bytes_received = recv(s, buf, BUFSZ, 0);

    if (bytes_received > 0) {
        // Copia o conteúdo recebido para 'mensagem'
        strncpy(mensagem, buf, sizeof(mensagem) - 1);
        mensagem[strcspn(mensagem, "\n")] = '\0';

        printf("comando recebido: %s\n", mensagem);
    }

}
	close(s);
    return 0;
    }




