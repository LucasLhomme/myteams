/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** socket with poll implementation
*/

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>

namespace Cli {
    class Socket {
        public:
            Socket();
            ~Socket();

            void create_socket();
            void create_sockaddr(const std::string& ip, const std::string& port);
            void connect_socket();
            void close_socket();

            /**
             * @brief Send data using poll for non-blocking I/O
             * @param data The data to send
             * @return true if successfully queued, false otherwise
             */
            bool send_data(const std::string& data);

            /**
             * @brief Receive data using poll for non-blocking I/O with timeout
             * @param timeout_ms Timeout in milliseconds (default 100ms)
             * @return The received data, or empty string if no data available
             */
            std::string receive_data(int timeout_ms = 100);

            /**
             * @brief Check if socket is ready for reading
             * @param timeout_ms Timeout in milliseconds
             * @return true if readable, false if timeout or error
             */
            bool is_readable(int timeout_ms = 100);

            /**
             * @brief Check if socket is ready for writing
             * @param timeout_ms Timeout in milliseconds
             * @return true if writable, false if timeout or error
             */
            bool is_writable(int timeout_ms = 100);

            int get_sockfd() const;
            bool is_connected() const;

        private:
            int _sockfd;
            struct sockaddr_in _server_addr;
            std::string _input_buffer;
            std::string _output_buffer;

            /**
             * @brief Internal method to perform poll on the socket
             * @param events POLLIN, POLLOUT, or both
             * @param timeout_ms Timeout in milliseconds
             * @return revents from poll, 0 if timeout, -1 if error
             */
            int _poll_socket(short events, int timeout_ms);

            /**
             * @brief Internal method to actually send queued data
             * @return true if all data sent or queued, false on error
             */
            bool _internal_send();

            /**
             * @brief Internal method to actually receive data
             * @return true if data received, false on error or no data
             */
            bool _internal_recv();
    };
}

#endif /* !SOCKET_HPP_ */
