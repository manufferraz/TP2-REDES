#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
  
#define TRUE   1
#define FALSE  0
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

 
int main(int argc , char **argv)
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , p2p_socket , client_socket[10] , max_clients = 10 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
    struct sockaddr_in address_p2p;
      
    char buffer[500]; 
      
    //set of socket descriptors
    fd_set readfds;
  
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) 
    {
        client_socket[i] = 0;
    }
      
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //create a p2p socket
    if( (p2p_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    //set master socket to allow multiple connections , this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
  
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[3]));

    //type of socket created
    address_p2p.sin_family = AF_INET;
    address_p2p.sin_addr.s_addr = INADDR_ANY;
    address_p2p.sin_port = htons(atoi(argv[2]));
      
    //bind the master socket to localhost port
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("master bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", address.sin_port);

    //bind the p2p socket to localhost port
    if (bind(p2p_socket, (struct sockaddr *)&address_p2p, sizeof(address_p2p))<0) 
    {
        perror("p2p bind failed");
        exit(EXIT_FAILURE);
    }
     
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
      
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("No peer found, starting to listen...");
     
    while(TRUE) 
    {
        //clear the socket set
        FD_ZERO(&readfds);
  
        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
         
        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++) 
        {
            //socket descriptor
            sd = client_socket[i];
             
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
  
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
          
        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(master_socket, &readfds)) 
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
              
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                     
                    break;
                } 
            }
        }
          
        char comando[BUFSZ]; // Variável para armazenar a instrução recebida

        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++) 
        {
            char buf[BUFSZ];
            char comando[BUFSZ]; // Variável para armazenar a instrução recebida

            memset(buf, 0, BUFSZ);
            sd = client_socket[i];
              
            if (FD_ISSET(sd , &readfds)) 
            {
                if ((valread = recv(sd , buffer, BUFSZ, 0)) == 0)
                {
                    close( sd );
                    client_socket[i] = 0;

                } else {
                    strncpy(comando, buffer, sizeof(comando) - 1);
                    comando[strcspn(comando, "\n")] = '\0';
                    printf("Comando recebido: %s\n", comando);
                    bzero(buffer, strlen(buffer)); // Limpa o buffer


                    // Trata o comando
                    if (strcmp(comando, "REQ_LS") == 0) {
                        char *mensagem = TrataLocalMaxSensor();
                        if (mensagem != NULL) {
                            send(sd, mensagem, strlen(mensagem), 0);
                            free(mensagem);
                        }
                    } else if (strcmp(comando, "REQ_LP") == 0) {
                        char *mensagem = TrataLocalPotency();
                            if (mensagem != NULL) {
                                send(sd, mensagem, strlen(mensagem), 0);
                                free(mensagem);
                            }
                    } else if (strcmp(comando, "REQ_ES") == 0) {
                        // Logica externa
                        char *mensagem = "REQ_LS";
                            if (mensagem != NULL) {
                                send(p2p_socket, mensagem, strlen(mensagem), 0);
                                char *msg_external = recv(p2p_socket , buffer, BUFSZ, 0);
                                send(sd, msg_external, strlen(msg_external), 0);
                                free(mensagem);
                                free(msg_external);
                            }
                    } else if (strcmp(comando, "REQ_EP") == 0) {
                        // Logica externa
                        char *mensagem = "REQ_LP";
                            if (mensagem != NULL) {
                                send(p2p_socket, mensagem, strlen(mensagem), 0);
                                char *msg_external = recv(p2p_socket , buffer, BUFSZ, 0);
                                send(sd, msg_external, strlen(msg_external), 0);
                                free(mensagem);
                                free(msg_external);
                            }
                    } else if (strcmp(comando, "REQ_MS") == 0) {
                        char *mensagem = TrataGlobalMaxSensor();
                            if (mensagem != NULL) {
                                send(sd, mensagem, strlen(mensagem), 0);
                                free(mensagem);
                            }
                    }else if (strcmp(comando, "REQ_MN") == 0) {
                        char *mensagem = TrataGlobalMaxSensor();
                            if (mensagem != NULL) {
                                send(sd, mensagem, strlen(mensagem), 0);
                                free(mensagem);
                            }
                    }     
                }
            } 
            // if
        }
    }
}