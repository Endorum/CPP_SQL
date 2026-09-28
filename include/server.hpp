#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>


std::string recieve_str(int sock);
int init_server(const std::string& ip, uint16_t port);