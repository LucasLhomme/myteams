/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server core with poll-based multi-client I/O management
*/

#include "server/server.hpp"
extern "C" {
#include "../../libs/myteams/logging_server.h"
}

#include <algorithm>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

extern bool server_running;

/**
 * @brief Construct Server with given port
 * @param port Port to listen on
 */
Server::Server(int port) : _port(port)
{
    load_state();
}

/**
 * @brief Destructor - cleanup all resources
 */
Server::~Server()
{
    cleanup();
}

/**
 * @brief Accept incoming client connection
 */
void Server::accept_client()
{
    try {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_socket = ::accept(
            _server_socket, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
        
        if (client_socket != -1) {
            _pollfds.push_back(make_pollfd(client_socket));
            _client_buffers[client_socket] = "";
            _output_buffers[client_socket] = "";
            std::cout << "New client connected on socket " << client_socket << std::endl;
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] accept_client exception: " << e.what() << std::endl;
    }
}

/**
 * @brief Send response to client (queues data for sending)
 * @param client_socket Socket to send to
 * @param message Message to send
 */
void Server::send_response(int client_socket, const std::string &message)
{
    try {
        std::string msg_to_send = message;
        if (msg_to_send.size() < 2 ||
            msg_to_send.substr(msg_to_send.size() - 2) != "\r\n") {
            msg_to_send += "\r\n";
        }
        _output_buffers[client_socket] += msg_to_send;
        
        for (auto &pfd : _pollfds) {
            if (pfd.fd == client_socket) {
                pfd.events |= POLLOUT;
                break;
            }
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] send_response exception: " << e.what() << std::endl;
    }
}

void Server::broadcast_event(const std::string &message, int exclude_socket)
{
    try {
        for (const auto &[socket, uuid] : _socket_to_uuid) {
            if (socket != exclude_socket) {
                send_response(socket, message);
            }
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] broadcast_event exception: " << e.what() << std::endl;
    }
}

/**
 * @brief Remove client from all tracking structures
 * @param client_socket Socket to remove
 */
void Server::remove_client(int client_socket)
{
    try {
        auto it_uuid = _socket_to_uuid.find(client_socket);
        if (it_uuid != _socket_to_uuid.end()) {
            std::string user_uuid = it_uuid->second;
            for (auto &user : _users) {
                char uuid_str[37];
                uuid_unparse(user->uuid, uuid_str);
                if (std::string(uuid_str) == user_uuid) {
                    user->is_online = false;
                    break;
                }
            }
            server_event_user_logged_out(user_uuid.c_str());
            
            std::string username = "";
            auto name_it = _socket_to_name.find(client_socket);
            if (name_it != _socket_to_name.end()) {
                username = name_it->second;
            }
            broadcast_event("EVENT\tLOGGED_OUT\t" + user_uuid + "\t" + username, client_socket);
            
            _socket_to_uuid.erase(it_uuid);
        }
        
        _socket_to_name.erase(client_socket);
        _use_team_context.erase(client_socket);
        _use_channel_context.erase(client_socket);
        _use_thread_context.erase(client_socket);
        _output_buffers.erase(client_socket);
        
        ::close(client_socket);
        _client_buffers.erase(client_socket);

        for (auto it = _pollfds.begin(); it != _pollfds.end(); ++it) {
            if (it->fd == client_socket) {
                _pollfds.erase(it);
                break;
            }
        }
        std::cout << "Client " << client_socket << " disconnected" << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[ERROR] remove_client exception: " << e.what() << std::endl;
    }
}

bool Server::handle_writable(size_t index) {
  try {
    int client_socket = _pollfds[index].fd;
    auto &buffer = _output_buffers[client_socket];

    if (buffer.empty()) {
      _pollfds[index].events &= ~POLLOUT;
      return false;
    }

    ssize_t sent = ::send(client_socket, buffer.c_str(), buffer.size(), MSG_NOSIGNAL);
    if (sent > 0) {
      buffer.erase(0, sent);
      if (buffer.empty()) {
        _pollfds[index].events &= ~POLLOUT;
      }
    } else if (sent == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
      remove_client(client_socket);
      return true;
    }
    return false;
  } catch (const std::exception &e) {
    std::cout << "[ERROR] handle_writable exception: " << e.what() << std::endl;
    return false;
  }
}

bool Server::handle_client_data(size_t poll_index) {
  try {
    int client_socket = _pollfds[poll_index].fd;
    char chunk[MAX_CHUNK_SIZE];
    ssize_t bytes_read = ::recv(client_socket, chunk, sizeof(chunk), 0);

    if (bytes_read <= 0) {
      remove_client(client_socket);
      return true;
    } else {
      _client_buffers[client_socket].append(chunk, bytes_read);
      extract_and_process_commands(client_socket);
      if (_client_buffers.find(client_socket) == _client_buffers.end())
        return true;
    }
    return false;
  } catch (const std::exception &e) {
    std::cout << "[ERROR] handle_client_data exception: " << e.what() << std::endl;
    return false;
  }
}

void Server::cleanup() {
  try {
    for (const auto &[client_socket, buffer] : _client_buffers) {
      (void)buffer;
      ::close(client_socket);
    }
    _client_buffers.clear();

    if (_server_socket != -1) {
      ::close(_server_socket);
      _server_socket = -1;
    }
    _pollfds.clear();
  } catch (const std::exception &e) {
    std::cout << "[ERROR] cleanup exception: " << e.what() << std::endl;
  }
}

int Server::start() {
  try {
    if (!init_socket() || !bind_socket() || !listen_socket()) {
      std::cout << "[ERROR] Initialization failed: " << strerror(errno) << std::endl;
      cleanup();
      return 84;
    }

    std::cout << "Server listening on port " << _port << std::endl;

    _pollfds.push_back(make_pollfd(_server_socket));

    while (server_running) {
      int poll_ret = ::poll(_pollfds.data(), _pollfds.size(), 100);
      if (poll_ret < 0) {
        if (errno == EINTR)
          continue;
        std::cout << "[ERROR] Poll failed: " << strerror(errno) << std::endl;
        break;
      }

      for (int i = static_cast<int>(_pollfds.size()) - 1; i >= 0; i--) {
        bool removed = false;
        if (_pollfds[i].revents & POLLIN) {
          if (_pollfds[i].fd == _server_socket) {
            accept_client();
          } else {
            removed = handle_client_data(i);
          }
        }
        if (!removed && (_pollfds[i].revents & POLLOUT)) {
          removed = handle_writable(i);
        }
      }
    }
    save_state();
    cleanup();
    return 0;
  } catch (const std::exception &e) {
    std::cout << "Server error: " << e.what() << std::endl;
    cleanup();
    return 84;
  }
}
