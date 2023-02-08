#include "client.h"

int main(int argc, char *argv[]) {
    int socket_d;
    struct sockaddr_in serverAddress;
    struct hostent *hp, *gethostbyname();

    if (argc < 4) {
        printf("ENTER ./client hostname port message\n");
        exit(1);
    }

    if ((socket_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &serverAddress.sin_addr, hp->h_length);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));

    if (connect(socket_d, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("client can not connect\n");
        exit(1);
    }

    printf("CLIENT: ready\n");
    char *message = argv[3];
    int amount_of_messages = atoi(argv[4]);
    for (int i = 0; i < amount_of_messages; i++) {
        sleep(i);
        if (send(socket_d, message, sizeof(message), 0) < 0) {
            perror("send error\n");
            exit(1);
        }
        printf("CLIENT: message has been sent: %s\n", argv[3]);
    }

    close(socket_d);
    return 0;
}