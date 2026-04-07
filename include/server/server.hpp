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
#include <unordered_map>
#include <vector>

class Server {
public:
  explicit Server(int port);
  ~Server();

  int start();

private:
  int _port;
  int _server_socket{-1};
  std::vector<struct pollfd> _pollfds;
  std::unordered_map<int, std::string> _client_buffers;
  std::unordered_map<int, std::string> _socket_to_name;

  using CommandHandlerFn = void (Server::*)(int,
                                            const std::vector<std::string> &);
  std::unordered_map<std::string, CommandHandlerFn> _command_map;

  void init_commands();
  void cmd_login(int client_socket, const std::vector<std::string> &args);
  void cmd_logout(int client_socket, const std::vector<std::string> &args);
  void cmd_users(int client_socket, const std::vector<std::string> &args);
  void cmd_user(int client_socket, const std::vector<std::string> &args);
  void cmd_send(int client_socket, const std::vector<std::string> &args);
  void cmd_messages(int client_socket, const std::vector<std::string> &args);
  void cmd_subscribe(int client_socket, const std::vector<std::string> &args);
  void cmd_subscribed(int client_socket, const std::vector<std::string> &args);
  void cmd_unsubscribe(int client_socket, const std::vector<std::string> &args);
  void cmd_help(int client_socket, const std::vector<std::string> &args);

  bool init_socket();
  bool bind_socket();
  bool listen_socket();

  void accept_client();
  void handle_client_data(size_t poll_index);
  void extract_and_process_commands(int client_socket);
  void process_command(int client_socket, const std::string &command);

  void send_response(int client_socket, const std::string &message);
  void remove_client(int client_socket);
  void cleanup();
  struct pollfd make_pollfd(int fd);
};

#endif /* !SERVER_HPP_ */
