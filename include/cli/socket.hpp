/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** socket
*/

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

namespace Cli {
    class Socket {
        public:
            Socket();
            ~Socket();

            void create_socket();
            void create_sockaddr(const std::string& ip, const std::string& port);
            void connect_socket();
            void close_socket();

            void send_data(const std::string& data);
            std::string receive_data(size_t max_size = 1024);

            int get_sockfd() const;

        private:
            int _sockfd;
            struct sockaddr_in _server_addr;
    };
}

#endif /* !SOCKET_HPP_ */
