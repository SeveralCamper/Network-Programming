#include "server.h"

using namespace std;

struct user
{
  string uip = "";
  int uport = 0;
  int usocket = 0;
  string username = "";
};

void print_users(user *users, ofstream &log)
{
  std::cout << "\033[0;34m";
  std::cout << "\t\tCONNECTED CLIENTS" << std::endl;
  log << "\t\tCONNECTED CLIENTS" << std::endl;
  std::cout << "\tUSERNAME SOCKET\t\tIP  PORT" << std::endl;
  log << "\tUSERNAME SOCKET\t\tIP  PORT" << std::endl;
  std::cout << "\e[m";

  for (int i = 0; i < MAX_CLIENTS; i++)
  {
    if (users[i].usocket != 0)
    {
      std::cout << setw(16) << users[i].username << setw(7) << users[i].usocket << setw(11) << users[i].uip << setw(6) << users[i].uport << std::endl;
      log << setw(16) << users[i].username << setw(7) << users[i].usocket << setw(11) << users[i].uip << setw(6) << users[i].uport << std::endl;
    }
  }
}

int main(int argc, char *argv[])
{
  ofstream log("log.txt", ios_base::app);
  if (!log.is_open())
  {
    cerr << "Error: could not open log" << std::endl;
    return 1;
  }

  log << "-----NEW SESSION (" << getTime() << ")-----" << std::endl;

  clear_screen();
  char *buffer = new char[BUFFER_SIZE];
  int server_socket, client_socket, bytes_recv;
  struct sockaddr_in server_address, client_address, arr_address[FD_SETSIZE];
  socklen_t address_length;
  fd_set read_fds, fd_list;
  user *users = new user[MAX_CLIENTS];
  bool system_call = false;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0)
  {
    cerr << "ERROR: Can't get a socket - " << strerror(errno) << std::endl;
    log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
            << std::endl;
    exit(EXIT_FAILURE);
  }

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(DEFAULT_SERVER_PORT);

  if (bind(server_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) < 0)
  {
    cerr << "ERROR: bind() failed - " << strerror(errno) << std::endl;
    log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
            << std::endl;
    exit(EXIT_FAILURE);
  }

  address_length = sizeof(server_address);
  if (getsockname(server_socket, (struct sockaddr *)&server_address,
                  &address_length) < 0)
  {
    cerr << "ERROR: getsockname() failed - " << strerror(errno) << std::endl;
    log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
            << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "SERVER: PORT - " << ntohs(server_address.sin_port) << std::endl;
  log << getTime() << "\t"
          << "SERVER: PORT - " << ntohs(server_address.sin_port) << std::endl;

  if (listen(server_socket, MAX_CLIENTS) < 0)
  {
    cerr << "ERROR: listen() failed - " << strerror(errno) << std::endl;
    log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
            << std::endl;
    exit(EXIT_FAILURE);
  }

  FD_ZERO(&fd_list);
  FD_SET(server_socket, &fd_list);
  FD_SET(STDIN_FILENO, &fd_list);

  while (true)
  {
    memcpy(&read_fds, &fd_list, sizeof(fd_list));
    if (select(FD_SETSIZE, &read_fds, nullptr, nullptr, nullptr) < 0)
    {
      cerr << "ERROR: select() failed - " << strerror(errno) << std::endl;
      log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
              << std::endl;
      exit(EXIT_FAILURE);
    }

    if (FD_ISSET(STDIN_FILENO, &read_fds))
    {
      string input;
      getline(cin, input);
      log << getTime() << "\tSERVER INPUT: " << input << std::endl;
      if (input == "exit")
      {
        close(server_socket);
        break;
      }
      if (input == "users")
      {
        print_users(users, log);
      }
      if (input.substr(0, 5) == "kick ")
      {
        string username = input.substr(5);
        bool kicked = false;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
          if (users[i].username == username)
          {
            string kick_message = "INFO: You have been kicked!";
            if (send(users[i].usocket, kick_message.c_str(), kick_message.size(), 0) < 0)
            {
              cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
              log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
                      << std::endl;
              exit(EXIT_FAILURE);
            }
            for (int j = 0; j < MAX_CLIENTS; j++)
            {
              if (users[j].usocket != users[i].usocket && users[j].usocket != 0)
              {
                string info_message = "INFO: " + users[i].username + " have been kicked!";
                if (send(users[j].usocket, info_message.c_str(), info_message.size(), 0) < 0)
                {
                  cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
                  log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
                          << std::endl;
                  exit(EXIT_FAILURE);
                }
              }
            }
            users[i].username = "";
            users[i].usocket = 0;
            kicked = true;
          }
        }
        if (!kicked)
        {
          cerr << "ERROR: Username not found" << std::endl;
          log << getTime() << "\tERROR: Username not found" << std::endl;
        }
      }
    }

    if (FD_ISSET(server_socket, &read_fds))
    {
      address_length = sizeof(client_address);
      client_socket = accept(server_socket,
                             (struct sockaddr *)&client_address,
                             (socklen_t *)&address_length);
      memcpy(&arr_address[client_socket], &client_address,
             sizeof(client_address));
      FD_SET(client_socket, &fd_list);

      std::cout << "\033[0;34m";
      std::cout << "SERVER: " << "\e[m" << "\033[0;3" << client_socket << "m" << inet_ntoa(client_address.sin_addr) << ":"
      << ntohs(client_address.sin_port) << " CONNECTED!" << "\e[m" << std::endl;
      log << getTime() << "\tSERVER: " << inet_ntoa(client_address.sin_addr) << ":"
              << ntohs(client_address.sin_port) << " CONNECTED!" << std::endl;

      for (int i = 0; i < MAX_CLIENTS; i++)
      {
        if (users[i].usocket == 0)
        {
          users[i].usocket = client_socket;
          users[i].uip = string(inet_ntoa(client_address.sin_addr));
          users[i].uport = ntohs(client_address.sin_port);
          break;
        }
      }
      memset(buffer, 0, sizeof(buffer));
    }

    for (int fd = 1; fd < FD_SETSIZE; fd++)
    {
      if (FD_ISSET(fd, &read_fds) && fd != server_socket)
      {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(fd, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0)
        {
          buffer[bytes_received] = '\0';
          memcpy(&client_address, &arr_address[fd], sizeof(arr_address[fd]));
          std::cout << "\033[0;33m";
          std::cout << "USRMSG: " << "\e[m" << "\033[0;3" << fd << "m" << inet_ntoa(client_address.sin_addr) << ":"
          << ntohs(client_address.sin_port) << " - " << buffer << "\e[m" << std::endl;
          log << getTime() << "\tUSRMSG: " << inet_ntoa(client_address.sin_addr)
                  << ":" << ntohs(client_address.sin_port) << " - " << buffer << std::endl;

          if (buffer[0] == '/' && buffer[1] == 's')
          {
            system_call = true;
          }
          else
          {
            system_call = false;
          }

          if (system_call)
          {
            if (buffer[3] == 'N')
            {
              string username = string(buffer + 5);
              for (int i = 0; i < MAX_CLIENTS; i++)
              {
                if (users[i].usocket == fd)
                {
                  for (int j = 0; j < MAX_CLIENTS; j++)
                  {
                    if (users[j].username == username)
                    {
                      string error_message = "ERROR: Username already taken";
                      if (send(users[i].usocket, error_message.c_str(), error_message.size(), 0) < 0)
                      {
                        cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
                        log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
                                << std::endl;
                        exit(EXIT_FAILURE);
                      }
                      break;
                    }
                  }
                  users[i].username = username;
                  string success_message = "SUCCESS: Username accepted";
                  if (send(users[i].usocket, success_message.c_str(), success_message.size(), 0) < 0)
                  {
                    cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
                    log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
                            << std::endl;
                    exit(EXIT_FAILURE);
                  }
                  break;
                }
              }
              memset(buffer, 0, sizeof(buffer));
            }
            else if (buffer[3] == 'W')
            {
              string new_buffer = string(buffer + 5);
              string sender, recipient, message;
              size_t arrow_pos = new_buffer.find("->");
              size_t colon_pos = new_buffer.find(": ");
              if (arrow_pos != string::npos && colon_pos != string::npos)
              {
                sender = new_buffer.substr(0, arrow_pos);
                recipient = new_buffer.substr(arrow_pos + 2, colon_pos - arrow_pos - 2);
                message = new_buffer.substr(colon_pos + 2);
              }

              int sender_socket, recipient_socket = -1;
              for (int i = 0; i < MAX_CLIENTS; i++)
              {
                if (users[i].username == sender)
                {
                  sender_socket = users[i].usocket;
                }
                if (users[i].username == recipient)
                {
                  recipient_socket = users[i].usocket;
                }
              }
              if (recipient_socket == -1 || sender == recipient)
              {
                string error_message;
                if (recipient_socket == -1)
                  error_message = "ERROR: Recipient doesn't exist";
                if (sender == recipient)
                  error_message = "ERROR: You try to whisper to yourself";
                cerr << error_message << std::endl;
                if (send(sender_socket, error_message.c_str(), error_message.size(), 0) < 0)
                {
                  cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
                  log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
                          << std::endl;
                  exit(EXIT_FAILURE);
                }
              }
              else
              {
                string direct_message = sender + " whispers to you: " + message;
                if (send(recipient_socket, direct_message.c_str(), direct_message.size(), 0) < 0)
                {
                  cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
                  log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
                          << std::endl;
                  exit(EXIT_FAILURE);
                }
              }
            }
          }

          for (int i = 0; i < FD_SETSIZE; i++)
          {
            if (FD_ISSET(i, &fd_list) && i != server_socket && i != fd && i != STDIN_FILENO)
            {
              if (!system_call)
              {
                if (send(i, buffer, strlen(buffer), 0) < 0)
                {
                  cerr << "ERROR: send() failed - " << strerror(errno) << std::endl;
                  log << getTime() << "\tERROR: send() failed - " << strerror(errno) << std::endl;
                }
              }
            }
          }
          memset(buffer, 0, sizeof(buffer));
        }
        else
        {
          FD_CLR(fd, &fd_list);
          std::cout << "\033[0;34m";
          std::cout << "SERVER: " << "\e[m" << "\033[0;3" << fd << "m" << inet_ntoa(client_address.sin_addr) << ":"
          << ntohs(client_address.sin_port) << " DISCONNECTED!" << "\e[m" << std::endl;
          log << getTime() << "\tSERVER: " << inet_ntoa(client_address.sin_addr) << ":"
                  << ntohs(client_address.sin_port) << " DISCONNECTED!" << std::endl;
          memset(&arr_address[fd], 0, sizeof(arr_address[fd]));
          close(fd);

          for (int i = 0; i < MAX_CLIENTS; i++)
          {
            if (users[i].usocket == fd)
            {
              users[i].usocket = 0;
              users[i].username = "";
            }
          }
        }
      }
    }
  }

  log << "-----SESSION ENDED (" << getTime() << ")-----\n\n\n"
          << endl;

  return EXIT_SUCCESS;
}
