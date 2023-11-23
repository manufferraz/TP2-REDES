#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFSZ 1024

void usage(int argc, char **argv)
{
    printf("usage: %s <server ip> <server port>\n", argv[0]);
    printf("example: %s 127.0.0.1 90100\n", argv[0]);
    exit(EXIT_FAILURE);
}

char* TrataLocalMaxSensor() 
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_LS");
        
        return comando;
}

char* TrataExternalMaxSensor() 
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_ES");
        
        return comando;
}

char* TrataLocalPotency() 
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_LP");
        
        return comando;
}

char* TrataExternalPotency() 
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_EP");
        
        return comando;
}

char* TrataGlobalMaxSensor() 
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_MS");
        
        return comando;
}

char* TrataGlobalMaxNetwork() 
{
        char *comando = (char *)malloc(BUFSZ); 
        snprintf(comando, BUFSZ, "REQ_MN");
        
        return comando;
}


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;                // estrutura de armazenamento p/ ipv6 ou ipv4
    if (0 != addrParse(argv[1], argv[2], &storage)) // parsing criada em common.c (útil tbm para server) do endereço para dentro da estrutura
    {
        usage(argc, argv);
    }
    
    // criação do socket
    int s = socket(storage.ss_family, SOCK_STREAM, 0); // CRIA SOCKET CONEXÃO INTERNET COM TCP (lib types e socket)
    if (s == -1)
    {
        perror("socket");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); // instanciação do endereço
    if (0 != connect(s, addr, sizeof(storage)))            // CONECTA NO SOCKET
    {
        perror("connect");
    }

    char addrstr[BUFSZ];
    // Configurar o socket 's' e conectar ao servidor

    int selected = 0;

    while (1) {
    char buf[BUFSZ];
    char string[BUFSZ]; // Variável para armazenar a string recebida
    char mensagem[BUFSZ]; // Variável para armazenar a mensagem recebida

    memset(buf, 0, BUFSZ);

    // Recebe o instrução do terminal
    fgets(buf, sizeof(buf), stdin);
    strncpy(string, buf, sizeof(string) - 1);

    bzero(buf, strlen(buf)); // Limpa o buffer

    // Remova a quebra de linha do final da string
    string[strcspn(string, "\n")] = '\0';

    // Processa a instrução e cria um array de elementos
    char *instrucao[BUFSZ];
    int numTokens = 0;

    for (int i = 0; i < BUFSZ; i++ ){
        instrucao[i] = "-1";
    }

    char temp[BUFSZ];
    strcpy(temp, string); 
    char *token = strtok(temp, " ");
    while (token != NULL) {
        instrucao[numTokens++] = token;
        token = strtok(NULL, " ");
    }


    // Verifica se a primeira posição do array é igual a "install"
    if (numTokens > 0 && strcmp(instrucao[0], "show") == 0) {
        if (numTokens > 0 && strcmp(instrucao[1], "localmaxsensor") == 0) {
            char *comando = TrataLocalMaxSensor();
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (numTokens > 0 && strcmp(instrucao[1], "externalmaxsensor") == 0) {
            char *comando = TrataExternalMaxSensor();
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (numTokens > 0 && strcmp(instrucao[1], "localpotency") == 0) {
            char *comando = TrataLocalPotency();
            if (comando != NULL) {
                printf("%s", comando);
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (numTokens > 0 && strcmp(instrucao[1], "externalpotency") == 0) {
            char *comando = TrataExternalPotency();
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (numTokens > 0 && strcmp(instrucao[1], "globalmaxsensor") == 0) {
            char *comando = TrataGlobalMaxSensor();
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (numTokens > 0 && strcmp(instrucao[1], "globalmaxnetwork") == 0) {
            char *comando = TrataGlobalMaxNetwork();
            if (comando != NULL) {
                send(s, comando, strlen(comando), 0);
                free(comando);
            }
        } else if (strcmp(string, "kill") == 0) {
            send(s, string, strlen(string), 0);
            close(s);
            printf("Servidor encerrado pelo cliente.\n");
            exit(EXIT_SUCCESS);
        } else {
            close(s);
            printf("Invalid command.\n");
            exit(EXIT_SUCCESS);
        }

        bzero(buf, strlen(buf));

        // Recebe a mensagem do servidor
        ssize_t bytes_received = recv(s, buf, BUFSZ, 0);

        if (bytes_received > 0) {
            // Copia o conteúdo recebido para 'mensagem'
            strncpy(mensagem, buf, sizeof(mensagem) - 1);
            mensagem[strcspn(mensagem, "\n")] = '\0';

            printf("mensagem recebida: %s\n", mensagem);
        }

    }
    }
	close(s);
    return 0;
    }




