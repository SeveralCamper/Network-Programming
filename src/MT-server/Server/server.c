#include "server.h"

int main() {
  int socket_d, socket_client, fd; // socket_d - сокет сервера, socket_client - сокет клиента, fd - файл для логирования информации
  socklen_t address_length; // address_length - длина адреса в битах
  struct sockaddr_in server_address, client_address; // server_address - адрес сервера, client_address - адрес клиента

  if ((socket_d = socket(AF_INET, SOCK_STREAM, 0)) < 0) { // socket(AF_INET, SOCK_STREAM, 0) - создает сокет
    perror("Error socket creation");                      // (AF_INET - задает домен соединения: выбирает набор протоколов, которые будут использоваться для создания соединения,
    exit(1);                                              // SOCK_STREAM - задает семантику коммутации (в нашем случае для TCP), 0 - задает конкретный проток, но обычно ставиться 0)
  }

  bzero((char *)&server_address, sizeof(server_address)); // bzero(void * s , size_t  n) - задает n байт в области пустыми
  server_address.sin_family = AF_INET;                    // Заполняем поля нашей стркутуры
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = 0;

  if (bind(socket_d, (struct sockaddr *)&server_address,
           sizeof(server_address)) < 0) {                 // привязывает к сокету локальный адрес
    perror("Error binding");
    exit(1);
  }

  address_length = sizeof(server_address);
  if (getsockname(socket_d, (struct sockaddr *)&server_address,
                  &address_length) < 0) {                 // возвращает текущее имя указанного сокета
    perror("Error getsocketname");
    exit(1);
  }

  printf("SERVER: port number: %d\n", ntohs(server_address.sin_port));
  if (listen(socket_d, 5) < 0) {                          // сообщяем ОС, что сервер готов слушать на сокете socket_d
    perror("Error listen");
    exit(1);
  }

  SocketClient thread_data;
  pthread_t thread;

  pthread_mutex_init(&mutex, NULL);                       // инициализируем mutex

  while (1) {                                             // 
    if ((socket_client = accept(socket_d, (struct sockaddr *)&client_address,
                                &address_length)) < 0) {
      perror("Error client socket");
      exit(1);
    }

    thread_data.socket_client = socket_client;
    thread_data.client_address = client_address;

    if (pthread_create(&thread, NULL, childWork, &thread_data) != 0) {
      perror("Error pthread create");
      exit(1);
    }

    if (pthread_detach(thread) != 0) {
      perror("Error pthread detach");
      exit(1);
    }
  }
  pthread_mutex_destroy(&mutex);
  close(fd);
  return 0;
}

void *childWork(void *args) {
  SocketClient *thread_data = (SocketClient *)args;
  int socket_client = thread_data->socket_client;
  struct sockaddr_in client_address = thread_data->client_address;
  int n;

  char *msg = malloc(sizeof(char) * BUF_SIZE);
  char *buf = malloc(sizeof(char) * BUF_SIZE);

  memset(msg, 0, BUF_SIZE * sizeof(char));

  printf("%s %s %s:%d %s\n", getTime(),
         "client address:", inet_ntoa(client_address.sin_addr),
         ntohs(client_address.sin_port), "connected");

  while (1) {
    if (recv(socket_client, msg, sizeof(msg), 0) <= 0) {
      break;
    }

    printf("%s %s %s:%d %s %s\n", getTime(),
           "client address:", inet_ntoa(client_address.sin_addr),
           ntohs(client_address.sin_port), "message:", msg);
    sprintf(buf, "%s %s %s:%d %s %s\n", getTime(),
            "client address:", inet_ntoa(client_address.sin_addr),
            ntohs(client_address.sin_port), "message:", msg);

    // pthread_mutex_lock(&mutex);
    int fd = open("log.txt", O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (fd < 0) {
      perror("Error open file");
      exit(1);
    }

    if (write(fd, buf, strlen(buf)) == -1) {
      perror("Error write");
      exit(1);
    }
    close(fd);
    // pthread_mutex_unlock(&mutex);
  }

  close(socket_client);
  printf("%s %s %s:%d %s\n", getTime(),
         "client address:", inet_ntoa(client_address.sin_addr),
         ntohs(client_address.sin_port), "disconnected");

  return NULL;
}

char *getTime() {
  time_t mytime = time(NULL);
  char *time_str = ctime(&mytime);
  time_str[strlen(time_str) - 1] = '\0';
  return time_str;
}
