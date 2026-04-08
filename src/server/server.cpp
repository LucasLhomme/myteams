/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server core
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

Server::Server(int port) : _port(port) {
  load_state();
  _pollfds.push_back(make_pollfd(STDIN_FILENO));
}
Server::~Server() { 
    cleanup(); 
}

void Server::accept_client() {
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
}

void Server::send_response(int client_socket, const std::string &message) {
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
}

void Server::remove_client(int client_socket) {
  auto it_uuid = _socket_to_uuid.find(client_socket);
  if (it_uuid != _socket_to_uuid.end()) {
    server_event_user_logged_out(it_uuid->second.c_str());
    _socket_to_uuid.erase(it_uuid);
  }
  _socket_to_name.erase(client_socket);
  ::close(client_socket);
  _client_buffers.erase(client_socket);

  for (auto it = _pollfds.begin(); it != _pollfds.end(); ++it) {
    if (it->fd == client_socket) {
      _pollfds.erase(it);
      break;
    }
  }
  std::cout << "Client " << client_socket << " disconnected" << std::endl;
}

bool Server::handle_writable(size_t index) {
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
}

bool Server::handle_client_data(size_t poll_index) {
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
}

void Server::cleanup() {
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
  _pollfds.push_back(make_pollfd(STDIN_FILENO));
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
          } else if (_pollfds[i].fd == STDIN_FILENO) {
            char buf[1];
            if (::read(STDIN_FILENO, buf, 1) <= 0) {
              server_running = false;
            }
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
