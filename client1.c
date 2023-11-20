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
    printf("example: %s 127.0.0.1 51511\n", argv[0]);
    exit(EXIT_FAILURE);
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
    char mensagem[BUFSZ]; // Variável para armazenar a mensagem recebida

    memset(buf, 0, BUFSZ);

    // Recebe a instrução do terminal
    fgets(buf, sizeof(buf), stdin);
    strncpy(string, buf, sizeof(string) - 1);

    bzero(buf, strlen(buf)); // Limpa o buffer

    // Remove a quebra de linha do final da string
    string[strcspn(string, "\n")] = '\0';

    bzero(buf, strlen(buf));

        send(s, string, strlen(string), 0);
     }
	close(s);
    return 0;
    }




