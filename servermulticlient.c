#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define TRUE 1
#define FALSE 0
#define BUFSZ 500

#define NUM_SENSORES 3
#define NUM_INFORMACOES 3

// Definir uma struct para representar um sensor
typedef struct {
    int id;
    double potencia;
    double eficiencia_energetica;
} Sensor;

// Array de sensores (declarado globalmente)
Sensor sensores[NUM_SENSORES] = {
    {1, 750, 80},
    {2, 1200, 95},
    {3, 500, 60}
};

char* TrataLocalMaxSensor() 
{
    // Encontrar o sensor com a maior potência útil
    double maxPotUtil = 0;
    int sensorId = -1;

    for (int i = 0; i < NUM_SENSORES; ++i) {
        double potUtil = sensores[i].potencia * sensores[i].eficiencia_energetica / 100.0;

        if (potUtil > maxPotUtil) {
            maxPotUtil = potUtil;
            sensorId = sensores[i].id;
        }
    }

    // Alocar espaço para a mensagem
    char *mensagem = (char *)malloc(BUFSZ); 
    // Construir a mensagem
    sprintf(mensagem, "local %d sensor %d: %.2f (%.2f %.2f)", sensores[sensorId - 1].id, sensorId, maxPotUtil, sensores[sensorId - 1].potencia, sensores[sensorId - 1].eficiencia_energetica);        
    
    return mensagem;
}

char* TrataExternalMaxSensor() 
{
    // Encontrar o sensor com a maior potência útil
    double maxPotUtil = 0;
    int sensorId = -1;

    for (int i = 0; i < NUM_SENSORES; ++i) {
        double potUtil = sensores[i].potencia * sensores[i].eficiencia_energetica / 100.0;

        if (potUtil > maxPotUtil) {
            maxPotUtil = potUtil;
            sensorId = sensores[i].id;
        }
    }

    // Alocar espaço para a mensagem
        char *mensagem = (char *)malloc(BUFSZ); 
    // Construir a mensagem
    sprintf(mensagem, "local %d sensor %d: %.2f (%.2f %.2f)", sensores[sensorId - 1].id, sensorId, maxPotUtil, sensores[sensorId - 1].potencia, sensores[sensorId - 1].eficiencia_energetica);        
    
    return mensagem;

}

char* TrataLocalPotency() 
{
double somatorio = 0;

    for (int i = 0; i < NUM_SENSORES; ++i) {
        somatorio += sensores[i].potencia * sensores[i].eficiencia_energetica / 100.0;
    }

    // Simula a criação da mensagem RES_LP
    char *mensagem = (char *)malloc(BUFSZ); 
    sprintf(mensagem, "local PidMi potency: %.2f", somatorio);

    return mensagem;
}

char* TrataExternalPotency() 
{

        char *mensagem = (char *)malloc(BUFSZ); 
        snprintf(mensagem, BUFSZ, "REQ_LP");
        
        return mensagem;

}

char* TrataGlobalMaxSensor() 
{

        char *mensagem = (char *)malloc(BUFSZ); 
        snprintf(mensagem, BUFSZ, "REQ_LP");
        
        return mensagem;

}

void handleClientConnections(int master_socket, int *client_socket, fd_set *readfds) {
    // Lógica para lidar com clientes regulares
    // ...

    // Exemplo:
    if (FD_ISSET(master_socket, readfds)) {
        // Código para aceitar conexões e tratar mensagens de clientes regulares
        // ...
    }
}

void handlePeerToPeerConnection(int p2p_socket, fd_set *p2p_readfds) {
    // Lógica para lidar com a conexão peer-to-peer
    // ...

    // Exemplo:
    if (FD_ISSET(p2p_socket, p2p_readfds)) {
        // Código para aceitar conexões e tratar mensagens peer-to-peer
        // ...
    }
}

int main(int argc, char **argv) {
    // ... (outras partes do seu código)

    int p2p_socket, new_p2p_socket;
    struct sockaddr_in p2p_address;

    // Crie o socket para conexões peer-to-peer
    if ((p2p_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("p2p socket failed");
        exit(EXIT_FAILURE);
    }

    p2p_address.sin_family = AF_INET;
    p2p_address.sin_addr.s_addr = INADDR_ANY;
    p2p_address.sin_port = htons(atoi(argv[4])); // Escolha uma porta diferente

    // Faça o bind do socket
    if (bind(p2p_socket, (struct sockaddr *)&p2p_address, sizeof(p2p_address)) < 0) {
        perror("p2p bind failed");
        exit(EXIT_FAILURE);
    }

    // Escute por conexões peer-to-peer
    if (listen(p2p_socket, 3) < 0) {
        perror("p2p listen failed");
        exit(EXIT_FAILURE);
    }

    int master_socket, addrlen, new_socket, client_socket[10], max_clients = 10, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    fd_set readfds;
    fd_set p2p_readfds;

    // Inicialize todos os sockets de clientes para 0
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Crie um master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = TRUE;
    // Permitir reutilização do endereço/porta
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[3]));

    // Faça o bind do master socket
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", address.sin_port);

    // Escute por até 3 conexões pendentes
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Aceite a conexão e envie/receba mensagens
    while (TRUE) {
        // Limpe os conjuntos de descritores
        FD_ZERO(&readfds);
        FD_ZERO(&p2p_readfds);

        // Adicione os sockets relevantes aos conjuntos de descritores
        FD_SET(master_socket, &readfds);
        FD_SET(p2p_socket, &p2p_readfds);
        max_sd = (master_socket > p2p_socket) ? master_socket : p2p_socket;

        // Adicione os sockets de clientes ao conjunto de descritores
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (sd > 0) {
                FD_SET(sd, &readfds);
                max_sd = (sd > max_sd) ? sd : max_sd;
            }
        }

        // Espera por atividade em qualquer um dos sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // Verifique a atividade no socket peer-to-peer
        handlePeerToPeerConnection(p2p_socket, &p2p_readfds);

        // Verifique a atividade nos sockets de clientes
        handleClientConnections(master_socket, client_socket, &readfds);
    }

    return 0;
}
