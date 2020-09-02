#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <time.h>

int main (int argc, char**argv){

    if(argc < 2){
        printf("Introduce la dirección y el puerto.\n");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result, *iterator;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;//PAra que pueda ser ipv4 o ipv6
    hints.ai_socktype = SOCK_DGRAM;//para UDP
    hints.ai_flags = AI_PASSIVE; //PAra que al incluir node=NULL lo sustituya por :: o 0.0.0.0

    if(getaddrinfo(argv[1], argv[2],&hints, &result) != 0){
        printf("ERROR: No se ha podido ejecutar el addrinfo.");
        exit(EXIT_FAILURE);
    }

    int socketUDP = socket(result->ai_family,
     result->ai_socktype,
     result->ai_protocol);

    if(bind(socketUDP, result->ai_addr, result->ai_addrlen) != 0){
        printf("ERROR: No se ha podido ejecutar el bind.");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    char cmdRcvd[2];
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr_storage client_addr;
    socklen_t client_addrlen = sizeof(client_addr);

    while (1)
    {
        ssize_t bytesRcv = recvfrom(socketUDP, cmdRcvd, 2, 0, (struct sockaddr *) &client_addr, &client_addrlen);
        cmdRcvd[1] = '\0';

        getnameinfo((struct sockaddr *) &client_addr, client_addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        printf("%i byte(s) de %s:%s\n", bytesRcv, host, serv);

        time_t tiempo = time(NULL);
        struct tm *tm = localtime(&tiempo);
        size_t max;
        char output[50];

        if(cmdRcvd[0] == 't'){
            size_t bytesT = strftime(output, max, "%I:%M:%S %p", tm);
            output[bytesT] = '\0';
            sendto(socketUDP, output, bytesT, 0, (struct sockaddr *) &client_addr, client_addrlen);
        }
        else if(cmdRcvd[0] == 'd'){
            size_t bytesT = strftime(output, max, "%Y-%m-%d", tm);
            output[bytesT] = '\0';
            sendto(socketUDP, output, bytesT, 0, (struct sockaddr *) &client_addr, client_addrlen);
        }
        else if(cmdRcvd[0] == 'q'){
            printf("Saliendo...\n");
            exit(0);
        }
        else{
            printf("Comando no implementado: %d...\n", cmdRcvd[0]);
        }
    }
    

    return 0;
}