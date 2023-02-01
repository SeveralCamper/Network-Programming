#include "client.h"

int main(int argc, char *argv[]) {

    int socket_d, bytes_read;
    struct sockaddr_in serverAddress, clientAddress;
    struct hostent *hp, *gethostbyname();
    socklen_t length;
    char *buf = (char *) malloc(sizeof(char) * BUF_SIZE);

    if (argc < 4) {
        printf("ENTER ./client hostname port message\n");
        exit(1);
    }

    if ((socket_d = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &serverAddress.sin_addr, hp->h_length);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));

    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddress.sin_port = 0;

    if (bind(socket_d, (struct sockaddr *) &clientAddress, sizeof(clientAddress)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("CLIENT: ready\n");
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int col = atoi(argv[3]);

    for (int i = 0; i < col; i++) {
        strcpy(buf, argv[3]);
        printf("CLIENT: message has been sent: %s\n", buf);
        if (sendto(socket_d, buf, strlen(buf), 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
            perror("sendto");
            exit(1);
        }

        if ((bytes_read = recvfrom(socket_d, buf, BUF_SIZE, 0, (struct sockaddr *) &serverAddress, &length)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        printf("CLIENT: message received: %s\n\n", buf);
        sleep(col);
    }
    close(socket_d);
    return 0;
}