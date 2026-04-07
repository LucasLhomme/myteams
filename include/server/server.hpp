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

#define MAX_NAME_LENGTH 32
#define MAX_DESCRIPTION_LENGTH 255
#define MAX_BODY_LENGTH 512

struct User {
  uuid_t uuid;
  char username[MAX_NAME_LENGTH];
  bool is_online;
};

struct Team {
  uuid_t uuid;
  char name[MAX_NAME_LENGTH];
  char description[MAX_DESCRIPTION_LENGTH];
};

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
  std::unordered_map<int, std::string> _output_buffers;
  std::unordered_map<int, std::string> _socket_to_name;
  std::unordered_map<int, std::string> _socket_to_uuid;

  std::vector<User *> _users;
  std::vector<Team *> _teams;

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
  bool handle_client_data(size_t poll_index);
  void extract_and_process_commands(int client_socket);

  void send_response(int client_socket, const std::string &message);
  bool handle_writable(size_t index);
  void remove_client(int client_socket);
  void cleanup();
  void save_state();
  void load_state();
  struct pollfd make_pollfd(int fd);
};

#endif /* !SERVER_HPP_ */
