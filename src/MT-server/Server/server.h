#ifndef SERVER_H_
#define SERVER_H_

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 128

pthread_mutex_t mutex;

typedef struct {
  int socket_client;
  struct sockaddr_in client_address;
} SocketClient;

void *childWork(void *args);
char *getTime();

#endif //  SERVER_H_