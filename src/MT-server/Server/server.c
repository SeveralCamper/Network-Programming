#include "server.h"

void reaper(int sig) {
    int status;
    while (wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}

int main()
{
    int sid = socket(AF_INET, SOCK_STREAM, 0);

    if (sid < 0) {
        perror("Failed to init socket!");
        exit(1);
    }

    struct sockaddr_in addr_p;

    addr_p.sin_family = AF_INET;
    addr_p.sin_port = 0;

    addr_p.sin_addr.s_addr = htonl(INADDR_ANY);

    inet_aton("127.0.0.1", &addr_p.sin_addr);

    if (bind(sid, (struct sockaddr *)&addr_p, sizeof(addr_p))) {
        perror("Failed bind name to socket!\n");
        exit(1);
    }

    socklen_t len = sizeof(addr_p);

    if (getsockname(sid, (struct sockaddr *)&addr_p, &len) < 0) {
        perror("Fail getsockname\n");
        exit(1);
    }


    printf("SERVER: port number - %d\n", ntohs(addr_p.sin_port));
    printf("SERVER: IP - %s\n", inet_ntoa(addr_p.sin_addr));

    if (listen(sid, 3)) {
        perror("Failed to init socket!\n");
        exit(1);
    }

     struct sockaddr_in client_addr_p;

     socklen_t lenC;

    (void) signal(SIGCHLD, reaper);

    while(1) {
        lenC = sizeof(client_addr_p);
        char buf[BUFLEN];
        int msg_l;


        int sidC = accept(sid, 0,  0);

        if (sidC < 0) {
            perror("Fail accept\n");
            exit(1);
        }

        switch(fork()) {
            case -1:
                perror("fork");
                close(sidC);
                break;
            case 0:
                close(sid);
                while(1) {
                    buf[0] = '\0';
                    if ((msg_l = recv(sidC, buf, BUFLEN, 0)) < 0) {
                        perror("Cannot read the message!\n");
                        exit(1);
                    }
                    if (buf[0] == '\0') {
                        close(sidC);
                        system("ps -aux | grep 'Z+'");
                        exit(0);
                    }
                    buf[msg_l] = '\0';
                    printf("/n");
                    printf("SERVER: message: %s\n", buf);
                    printf("SERVER: message lenght: %d\n", msg_l);
                    printf("SERVER: process ID: %d\n", getpid());
                    printf("/n");
                }
            default:
                close(sidC);
                while(waitpid(-1, NULL, WNOHANG) > 0);



        }
    }
}
