#include "../include/client.h"



int get_server_port() {
  FILE *fp = popen("lsof -iTCP -sTCP:LISTEN | grep server", "r");
  if (!fp) {
    std::cerr << "ERROR: popen() failed - " << strerror(errno) << std::endl;
    return -1;
  }
  int port = 0;
  char line[256];
  while (fgets(line, sizeof(line), fp)) {
    char *pch = strchr(line, ':');
    if (pch != NULL) {
      port = atoi(pch + 1);
      break;
    }
  }
  pclose(fp);
  return port;
}

void set_username(std::string &username, int client_socket) {
  while (true) {
    std::cout << "Please enter your username: ";
    std::getline(std::cin, username);
    if (username.find(' ') != std::string::npos || username == "") {
      std::cerr << "ERROR: Spaces in username or empty username is not allowed! " << std::endl;
      username = "";
    } else {
      std::string username_message = "/s N " + username;
      if (send(client_socket, username_message.c_str(), username_message.size(), 0) < 0) {
        std::cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
      }

      char buffer[BUFFER_SIZE];
      int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
      if (bytes_received < 0) {
        std::cerr << "ERROR: recv() failed - " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
      } else if (bytes_received == 0) {
        std::cerr << "ERROR: Connection closed by server" << std::endl;
        exit(EXIT_FAILURE);
      } else {
        buffer[bytes_received] = '\0';
        if (strcmp(buffer, "SUCCESS: Username accepted") == 0) {
          std::cout << "Username accepted" << std::endl;
          print_info();
          break;
        } else {
          std::cerr << "ERROR: Username already taken" << std::endl;
          username = "";
        }
      }
    }
  }
}

void receive_messages(int client_socket) {
  while (true) {
    char *buffer = new char[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
      std::cerr << "ERROR: recv() failed - " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      std::cout << "INFO: Connection closed by server" << std::endl;
      close(client_socket);
      exit(EXIT_SUCCESS);
    } else {
      std::string temp = std::string(buffer);
      if (strcmp(buffer, "ERROR: Username already taken") == 0) {
        std::string new_username = "";
        set_username(new_username, client_socket);
      }
      buffer[bytes_received] = '\0';
      std::cout << buffer << std::endl;
      if (strcmp(buffer, "INFO: You have been kicked!") == 0) {
        close(client_socket);
        exit(EXIT_SUCCESS);
      }
    }
    memset(buffer, 0, sizeof(buffer));
  }
}

int main(int argc, char *argv[]) {
  clear_screen();
  int client_socket, current_room = 0;
  struct sockaddr_in server_address, client_address;
  struct hostent *hp, *gethostbyname();

  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "ERROR: Can't get a socket - " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  client_address.sin_family = AF_INET;
  client_address.sin_addr.s_addr = INADDR_ANY;
  client_address.sin_port = htonl(0);

  socklen_t address_length = sizeof(client_address);
  if (getsockname(client_socket, (struct sockaddr *)&client_address, &address_length) < 0) {
    std::cerr << "ERROR: getsockname() failed - " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  if (DEFAULT_SERVER_PORT == 0) {
    server_address.sin_port = htons(get_server_port());
  } else {
    server_address.sin_port = htons(DEFAULT_SERVER_PORT);
  }

  inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

  if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    std::cerr << "ERROR: connect() failed - " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  } else {
    std::cout << "Connected to 127.0.0.1:" << ntohs(server_address.sin_port) << std::endl;
  }

  std::string username = "";
  set_username(username, client_socket);

  std::string room = "[MAIN]";

  std::thread receiver(receive_messages, client_socket);
  receiver.detach();

  while (true) {
    std::string message;
    std::getline(std::cin, message);

    if (message == "/exit") {
      break;
    } else if (message == "/help") {
      print_info();
    } else if (message.substr(0, 2) == "/d") {
      std::string recipient = "", direct_message = "";
      size_t space_pos = message.find_first_of(' ', 3);

      if (space_pos == std::string::npos) {
        std::cerr << "ERROR: Invalid input" << std::endl;
      } else {
        recipient = message.substr(3, space_pos - 3);
        direct_message = message.substr(space_pos + 1);
        std::string dm_message = "/s W " + username + "->" + recipient + ": " + direct_message;
        if (send(client_socket, dm_message.c_str(), dm_message.size(), 0) < 0) {
          std::cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
          exit(EXIT_FAILURE);
        }
      }
    } else if (message.substr(0, 3) == "/cr") {
      current_room =  std::stoi(message.substr(4, 5));
      if (current_room == SERVER_ROOMS::ROOM_1) {
        room = "[R1]";
      } else if (current_room == SERVER_ROOMS::ROOM_2) {
        room = "[R2]";
      } else if (current_room == SERVER_ROOMS::ROOM_3) {
        room = "[R3]";
      } else if (current_room == SERVER_ROOMS::PRIVATE) {
        room = "[P]";
      } else {
        room = "[M]";
      }
      std::cout << "You have joined to" << room << std::endl;
    } else if (message.find('/') == 0) {
      std::cout << "ERROR: Use /help to see availabe commands!" << std::endl;
    } else {
      std::string full_message = room + " " + username + ": " + message;
      if (send(client_socket, full_message.c_str(), full_message.size(), 0) < 0) {
        std::cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }

  std::cout << "CLIENT: Connection closed" << std::endl;

  close(client_socket);

  return EXIT_SUCCESS;
}
