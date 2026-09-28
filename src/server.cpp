#include <iostream>

#include "../include/server.hpp"

// blocks until a client connects
std::string recieve_str(int sock){
    
    int client = accept(sock, nullptr, nullptr);

    if(client < 0){
        std::cerr << "Error while accepting: " << strerror(errno) << std::endl;
        return {};
    }

    std::string msg;

    char buf[1024];
    ssize_t n;
    while((n = recv(client, buf, sizeof(buf), 0)) > 0){
        msg.append(buf, n);
    }

    if (n < 0){
        std::cerr << "Error while receiving: " << strerror(errno) << std::endl;
        return {};
    }

    close(client);
    return msg;
}


int init_server(const std::string& ip, uint16_t port){

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if(sock < 0){
        std::cerr << "Error while creating socket: " << strerror(errno) << std::endl;
        return -1;
    }

    int yes = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0){
        std::cerr << "Error while setting socket options: " << strerror(errno) << std::endl;
        close(sock);
        return -1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET; 
    addr.sin_port = htons(port);

    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1){
        std::cerr << "Error while converting IP address: " << ip << " -> " << strerror(errno) << std::endl;
        close(sock);
        return -1;
    }


    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        std::cerr << "Error while binding: " << strerror(errno) << std::endl;
        close(sock);
        return -1;
    }

    if(listen(sock, 10) < 0){
        std::cerr << "Error while listening: " << strerror(errno) << std::endl;
        close(sock);
        return -1;  
    }

    return sock;
}