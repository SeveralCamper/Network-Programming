#include "server.h" 

int main() {

    int socket_d, messageLength; // soket ID, length of incoming message
    socklen_t addressLength; // length of address
    struct sockaddr_in serverAddress, clientAddress; // address of server, address of client
    char *buf = malloc(sizeof(char) * BUF_SIZE);

    if ((socket_d = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation error");
        exit(1);
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = 0;

    if (bind(socket_d, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("binding error");
        exit(1);
    }

    addressLength = sizeof(serverAddress);
    if (getsockname(socket_d, (struct sockaddr *) &serverAddress, &addressLength) < 0) {
        perror("Error getsocketname");
        exit(1);
    }

    printf("SERVER: port number: %d\n", ntohs(serverAddress.sin_port));
    while (1) {
        addressLength = sizeof(clientAddress);
        bzero(buf, BUF_SIZE);
        if ((messageLength = recvfrom(socket_d, buf, BUF_SIZE, 0, (struct sockaddr *) &clientAddress, &addressLength)) < 0) {
            perror("Error socket client");
            exit(1);
        }

        printf("SERVER: client address: %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
        printf("SERVER: client message: %s\n", buf);
        buf = strcat(buf, " :mess");

        if (sendto(socket_d, buf, sizeof(buf), 0, (struct sockaddr *) &clientAddress, sizeof(clientAddress)) < 0) {
            perror("Sending error");
            exit(1);
        }
        printf("SERVER: message has been changed to: %s\n\n", buf);
    }
    close(socket_d);
    return 0;
}