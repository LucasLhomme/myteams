/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** server
*/

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <poll.h>
#include <string>
#include <sys/queue.h>
#include <unordered_map>
#include <uuid/uuid.h>
#include <vector>

#include "constants.hpp"
#include "user.hpp"
#include "team.hpp"
#include "commands/CommandFactory.hpp"

class Server {
public:
  explicit Server(int port);
  ~Server();

  int start();

  const std::vector<User *> &get_users() const { return _users; }
  std::vector<User *> &get_users() { return _users; }
  
  std::unordered_map<int, std::string> &get_socket_to_name() { return _socket_to_name; }
  std::unordered_map<int, std::string> &get_socket_to_uuid() { return _socket_to_uuid; }

  void send_response(int client_socket, const std::string &message);
  void save_state();
  
  std::vector<std::string> get_all_usernames();
  void GetUserInfo(int client_socket, const std::string &uuid_str);

private:
  int _port;
  int _server_socket{-1};
  std::vector<struct pollfd> _pollfds;
  std::unordered_map<int, std::string> _client_buffers;
  std::unordered_map<int, std::string> _output_buffers;
  std::unordered_map<int, std::string> _socket_to_name;
  std::unordered_map<int, std::string> _socket_to_uuid;

  std::vector<User *> _users;
  std::vector<Team *> _teams;

  using CommandHandler= void (Server::*)(int, const std::vector<std::string> &);
  std::unordered_map<std::string, CommandHandler> _command_map;
  CommandFactory _command_factory;

  bool init_socket();
  bool bind_socket();
  bool listen_socket();

  void accept_client();
  bool handle_client_data(size_t poll_index);
  void extract_and_process_commands(int client_socket);

  bool handle_writable(size_t index);
  void remove_client(int client_socket);
  void cleanup();
  void load_state();
  struct pollfd make_pollfd(int fd);
};

#endif /* !SERVER_HPP_ */
