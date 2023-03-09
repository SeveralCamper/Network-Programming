#include "client.h"

int main(int argc, char* argv[]){
    if (argc < 4) {
        printf("./client [IP-адрес сервера] [порт сервера] [сообщение]");
        exit(1);
    }

    int sid = socket(AF_INET, SOCK_STREAM, 0);
    if (sid < 0) {
        perror("Cannot create socket!");
        exit(1);
    }

    struct sockaddr_in addr_p;

    addr_p.sin_family = AF_INET;
    addr_p.sin_port = 0;
    addr_p.sin_addr.s_addr = htonl(INADDR_ANY);

    socklen_t len = sizeof(addr_p);

    if (getsockname(sid, (struct sockaddr *)&addr_p, &len) < 0) {
        perror("Fail getsockname\n");
        exit(1);
    }

    printf("CLIENT: port number - %d\n", ntohs(addr_p.sin_port));
    printf("СLIENT: IP - %s\n", inet_ntoa(addr_p.sin_addr));

    struct sockaddr_in server_addr_p;

    server_addr_p.sin_family = AF_INET;
    server_addr_p.sin_port = htons(atoi(argv[2]));

    inet_pton(AF_INET, argv[1], &server_addr_p.sin_addr);

    int p = connect(sid, (struct sockaddr *) &server_addr_p, sizeof(server_addr_p));

    if (p < 0) {
        perror("Fail connect to server\n");
        exit(1);
    }

    printf("Message forwarding have been started!\n");

    char msg[10] = "Hello!";

    int i = 0;
    for (int i = 0; i < atoi(argv[3]) + 5; i++) {
        sprintf(msg, "%d", i);
        if (send(sid, msg, strlen(msg), 0) < 0) {
            perror("Cannot send message!\n");
            exit(1);
        }
        printf("CLIENT: IP server - : %s\n", inet_ntoa(server_addr_p.sin_addr));
        printf("CLIENT: port server - %d\n", ntohs(server_addr_p.sin_port));
        printf("CLIENT: message: %s\n", msg);
         printf("\nSend value %d\n\n", i);
        sleep(i);
    }
    
    printf("Message forwarding have been finished!\n");

    

    close(sid);

}