#include <arpa/inet.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 128

char *getTime() {
  time_t mytime = time(NULL);
  char *time_str = ctime(&mytime);
  time_str[strlen(time_str) - 1] = '\0';
  return time_str;
}


int main(void) {
  int client_socket, server_socket;
  socklen_t address_length;
  struct sockaddr_in server_address, client_address, arr_address[FD_SETSIZE];

  {
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Error socket creation");
      exit(1);
    }

    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = 0;

    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0) {
      perror("Error binding");
      exit(1);
    }

    address_length = sizeof(server_address);
    if (getsockname(server_socket, (struct sockaddr *)&server_address,
                    &address_length) < 0) {
      perror("Error getsocketname");
      exit(1);
    }

    printf("SERVER: port number: %d\n", ntohs(server_address.sin_port));
    if (listen(server_socket, 5) < 0) {
      perror("Error listen");
      exit(1);
    }
  }

  fd_set readfds, fdlist;
  FD_ZERO(&fdlist);
  FD_SET(server_socket, &fdlist);

  char *buffer = (char *) malloc(BUF_SIZE);
  while (1) {
    memcpy(&readfds, &fdlist, sizeof(fdlist));
    select(FD_SETSIZE, &readfds, NULL, NULL, NULL);

    if (FD_ISSET(server_socket, &readfds)) {
      client_socket = accept(server_socket, (struct sockaddr *)(&client_address), &address_length);
      memcpy(&arr_address[client_socket], &client_address, sizeof(client_address));
      FD_SET(client_socket, &fdlist);
      printf("\033[0;3%dm", client_socket);
      printf("%s %s %s:%d %s\n", getTime(), "Client address:", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), "connected");
    }
   

    for (int fd = 0; fd < FD_SETSIZE; fd++) {
      if (FD_ISSET(fd, &readfds) && fd != server_socket) {
        if (recv(fd, buffer, BUF_SIZE, 0) > 0) {
          memcpy(&client_address, &arr_address[fd], sizeof(arr_address[fd]));
          printf("\033[0;3%dm", fd);
          printf("%s %s %s:%d %s %s\n", getTime(), "Client address:", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), "message:", buffer);
        } else {
          FD_CLR(fd, &fdlist);
          printf("\033[0;3%dm", fd);
          printf("%s %s %s:%d %s\n", getTime(), "Client address:", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), "disconnected");
          memset(&arr_address[fd], 0, sizeof(arr_address[fd]));
          close(fd);
        }
      }
    }
  }

  return 0;
}

