/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** client
*/
#include "./cli/client.hpp"
#include "./cli/commands.hpp"
#include "./cli/client_poll_state.hpp"
#include "./cli/socket.hpp"
#include "exception.hpp"
#include <iostream>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sstream>
#include <vector>

extern "C" {
#include "../../libs/myteams/logging_client.h"
}

Cli::ClientPollState::ClientPollState(int server_sockfd) : stdin_fd{STDIN_FILENO, POLLIN, 0}, server_fd{server_sockfd, POLLIN | POLLHUP | POLLERR, 0} {

}

std::array<pollfd, 2> Cli::ClientPollState::to_array() const
{
    return {stdin_fd, server_fd};
}


namespace Cli {
    void Client::launch_client(std::string ip, std::string port) {
        try {
            CommandHandler command_handler;
            Socket client_socket;

            try {
                client_socket.create_socket();
                client_socket.create_sockaddr(ip, port);
                client_socket.connect_socket();
                std::cout << "Client socket connected successfully to " << ip << ":" << port
                        << std::endl;
            } catch (const Exception &e) {
                std::cout << e.what() << std::endl;
                exit(84);
            }

            while (true) {
                ClientPollState poll_state(client_socket.get_sockfd());
                auto fds = poll_state.to_array();

                int poll_ret = poll(fds.data(), fds.size(), -1);
                if (poll_ret < 0) {
                    if (errno == EINTR)
                        continue;
                    std::cout << "Poll error: " << std::strerror(errno) << std::endl;
                    break;
                }

                if (fds[1].revents & (POLLHUP | POLLERR | POLLNVAL)) {
                    std::cout << "Server disconnected" << std::endl;
                    break;
                }

                if (fds[1].revents & POLLIN) {
                    while (client_socket.is_readable(0)) {
                        const std::string line = client_socket.receive_data(1);

                        if (line.empty() || !client_socket.is_connected()) {
                            break;
                        }
                        if (!CommandHandler::handle_async_event(line)) {
                            std::cout << line << std::endl;
                        }
                    }
                    if (!client_socket.is_connected()) {
                        std::cout << "Server disconnected" << std::endl;
                        break;
                    }
                }

                if (fds[0].revents & POLLIN) {
                    std::string input;
                    if (!std::getline(std::cin, input)) {
                        break;
                    }
                    try {
                        command_handler.process_command(input, client_socket, _context);
                        if (_context.should_quit()) {
                            break;
                        }
                    } catch (const Exception &e) {
                        std::cout << e.what() << std::endl;
                        break;
                    }
                }
            }
            client_socket.close_socket();
            std::cout << "Client socket closed successfully" << std::endl;
        } catch (const std::exception &e) {
            std::cout << "[ERROR] launch_client exception: " << e.what() << std::endl;
        }
    }
} // namespace Cli