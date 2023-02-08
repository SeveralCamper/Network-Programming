#include "server.h"

void reaper(int sig)
{
    int status;
    while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}

void childWork(int socket_client, struct sockaddr_in clientAddress) {
    int messageLength;
    char *buf = malloc(sizeof(char) * BUF_SIZE);

    while((messageLength = recv(socket_client, buf, BUF_SIZE, 0)) > 0) {
        printf("SERVER: client address: %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
        printf("SERVER: client message: %s\n\n", buf);
    }
    printf("SERVER: client address connection closed: %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));

}

int main() {

    int socket_d, socket_client, child;
    socklen_t addressLength;
    struct sockaddr_in serverAddress, clientAddress;

    if ((socket_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
    listen(socket_d, 5);
    signal(SIGCHLD, reaper);
    while (1) {
        if ((socket_client = accept(socket_d, (struct sockaddr *) &clientAddress, &addressLength)) < 0) {
            perror("Wrong client socket");
            exit(1);
        }

        if ((child = fork()) < 0) {
            perror("fork error");
            exit(1);
        }

        if (child == 0) {
            close(socket_d);
            childWork(socket_client, clientAddress);
            close(socket_client);
        }
        close(socket_client);
    }
}