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

void print_info() {
    std::cout << std::endl << "HELP INFO" << std::endl << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "/help - print this information" << std::endl;
    std::cout << "/exit - close connection and close a programm" << std::endl;
    std::cout << "/w <username> <message> - direct message" << std::endl << std::endl;
    std::cout << "The tools used:" << std::endl;
    std::cout << "*   Connection protocol - TCP" << std::endl;
    std::cout << "*   Multithreaded implementation - select" << std::endl;
}

void clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}
