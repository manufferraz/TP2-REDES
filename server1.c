#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 500


void usage(int argc, char **argv)
{
    printf("usage: %s <v4|v6> <server port>\n", argv[0]);
    printf("example: %s v4 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;                           // estrutura de armazenamento p/ ipv6 ou ipv4
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) // parsing criada em common.c (útil tbm para server) do endereço para dentro da estrutura
    {
        usage(argc, argv);
    }

    // criação do socket
    int s = socket(storage.ss_family, SOCK_STREAM, 0); // CRIA SOCKET CONEXÃO INTERNET COM TCP (lib types e socket)
    if (s == -1)
    {
        logExit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) // reutilizar porto
    {
        logExit("setsocketopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage); // instanciação do endereço
    if (0 != bind(s, addr, sizeof(storage)))
    {
        logExit("bind");
    }

    if (0 != listen(s, 10))
    {
        logExit("listen");
    }

    int csock;

    char addrstr[BUFSZ];
    addrToStr(addr, addrstr, BUFSZ);
    printf("bound to %s, waiting connections\n", addrstr);

    struct sockaddr_storage cstorage;
    struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
    socklen_t caddrlen = sizeof(cstorage);

    int flag = 0;

     while (1) {
        if(flag == 0){
        csock = accept(s, caddr, &caddrlen);
            if (csock == -1)
            {
                logExit("accept");
        }

        char caddrstr[BUFSZ];
        addrToStr(caddr, caddrstr, BUFSZ);
        printf("[log] connection from %s\n", caddrstr);

        flag = 1;
        }

        char buf[BUFSZ];
        char comando[BUFSZ]; // Variável para armazenar a instrução recebida

        memset(buf, 0, BUFSZ);

        // Recebe o comando do cliente

        ssize_t bytes_received = recv(csock, buf, BUFSZ, 0);

        if (bytes_received == 0 ){
            flag = 0;
        } else if (bytes_received > 0) {
            // Copia o conteúdo recebido para 'comando'
            strncpy(comando, buf, sizeof(comando) - 1);
            comando[strcspn(comando, "\n")] = '\0';
            printf("Comando recebido: %s\n", comando);
        }
    }
    close(csock);
    return 0;
}
