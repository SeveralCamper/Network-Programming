#pragma once 

#include <arpa/inet.h>
#include <iostream>
#include <iomanip>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <fstream>

#define DEFAULT_SERVER_PORT 0
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void clear_screen()
{
    std::cout << "\033[2J\033[1;1H";
}

char *getTime()
{
     time_t mytime = time(NULL);
     char *time_str = ctime(&mytime);
     time_str[strlen(time_str) - 1] = '\0';
     return time_str;
}