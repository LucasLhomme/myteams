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

static std::vector<std::string> split_tab_fields(const std::string &line)
{
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);

    while (std::getline(stream, field, '\t')) {
        fields.push_back(field);
    }
    return fields;
}

static bool handle_async_event(const std::string &line)
{
    const std::vector<std::string> fields = split_tab_fields(line);

    if (fields.size() < 2 || fields[0] != "EVENT") {
        return false;
    }

    if (fields[1] == "LOGGED_IN" && fields.size() >= 4) {
        client_event_logged_in(fields[2].c_str(), fields[3].c_str());
        return true;
    }
    if (fields[1] == "LOGGED_OUT" && fields.size() >= 4) {
        client_event_logged_out(fields[2].c_str(), fields[3].c_str());
        return true;
    }
    if (fields[1] == "MESSAGE_RECEIVED" && fields.size() >= 4) {
        client_event_private_message_received(fields[2].c_str(), fields[3].c_str());
        return true;
    }
    if (fields[1] == "TEAM_CREATED" && fields.size() >= 5) {
        client_event_team_created(fields[2].c_str(), fields[3].c_str(), fields[4].c_str());
        return true;
    }
    if (fields[1] == "CHANNEL_CREATED" && fields.size() >= 5) {
        client_event_channel_created(fields[2].c_str(), fields[3].c_str(), fields[4].c_str());
        return true;
    }
    if (fields[1] == "THREAD_CREATED" && fields.size() >= 7) {
        const time_t timestamp = static_cast<time_t>(std::stoll(fields[4]));

        client_event_thread_created(fields[2].c_str(), fields[3].c_str(), timestamp,
            fields[5].c_str(), fields[6].c_str());
        return true;
    }
    if (fields[1] == "THREAD_REPLY_RECEIVED" && fields.size() >= 6) {
        client_event_thread_reply_received(fields[2].c_str(), fields[3].c_str(),
            fields[4].c_str(), fields[5].c_str());
        return true;
    }
    return false;
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