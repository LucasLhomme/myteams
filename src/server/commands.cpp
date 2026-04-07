/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** commands registry and parser
*/

#include "server/server.hpp"
#include <regex>
#include <iostream>

void Server::init_commands()
{
    _command_map["/login"] = &Server::cmd_login;
    _command_map["/logout"] = &Server::cmd_logout;
    _command_map["/users"] = &Server::cmd_users;
    _command_map["/user"] = &Server::cmd_user;
    _command_map["/send"] = &Server::cmd_send;
    _command_map["/messages"] = &Server::cmd_messages;
    _command_map["/subscribe"] = &Server::cmd_subscribe;
    _command_map["/subscribed"] = &Server::cmd_subscribed;
    _command_map["/unsubscribe"] = &Server::cmd_unsubscribe;
    _command_map["/help"] = &Server::cmd_help;
    _command_map["exit"] = &Server::cmd_logout;
}

void Server::extract_and_process_commands(int client_socket) {
  auto &buffer = _client_buffers[client_socket];
  size_t pos;

  while ((pos = buffer.find("\r\n")) != std::string::npos) {
    std::string command = buffer.substr(0, pos);
    buffer.erase(0, pos + 2);

    while (!command.empty() && std::isspace(static_cast<unsigned char>(command.back())))
      command.pop_back();
    if (command.empty()) continue;

    std::vector<std::string> args;
    // this regex is for parsing arguments with quotes
    std::regex token_regex(R"(("[^"]*"|\S+))");
    auto it = std::sregex_iterator(command.begin(), command.end(), token_regex);
    auto end = std::sregex_iterator();

    for (; it != end; ++it) {
      std::string token = it->str();
      if (token.size() >= 2 && token.front() == '"' && token.back() == '"')
        token = token.substr(1, token.size() - 2);
      args.push_back(token);
    }
    if (args.empty()) continue;

    std::cout << "Processing command: [" << args[0] << "]" << std::endl;
    auto map_it = _command_map.find(args[0]);
    if (map_it != _command_map.end()) {
      (this->*(map_it->second))(client_socket, args);
    } else {
      send_response(client_socket, "Unknown command");
    }
  }
  if (buffer.size() > 4096)
    buffer.clear();
}
