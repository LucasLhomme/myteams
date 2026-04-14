/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** commands registry and parser
*/

#include "server/server.hpp"
#include <regex>
#include <iostream>

static bool is_public_command(const std::string &command)
{
  return command == "/login" || command == "/help";
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

    //log
    auto iterrator = _socket_to_name.find(client_socket);
    if (iterrator != _socket_to_name.end())
        std::cout << "User: " << iterrator->second << " send processing command " << args[0] << std::endl;
    else
        std::cout << "User: [unknown] send processing command " << args[0] << std::endl;
    
    const bool is_logged_in = (_socket_to_uuid.find(client_socket) != _socket_to_uuid.end());

    if (!is_logged_in && !is_public_command(args[0])) {
        send_response(client_socket, "You must be logged in to use this command. Use /login or /help");
        continue;
    }
        
    auto map_it = _command_map.find(args[0]);

    try {
        if (auto generic_cmd = _command_factory.get_command(args[0])) {
            generic_cmd->execute(*this, client_socket, args);
        } else if (map_it != _command_map.end()) {
          (this->*(map_it->second))(client_socket, args);
        } else {
          send_response(client_socket, "Unknown command");
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] Exception during command '" << args[0] << "': " << e.what() << std::endl;
        send_response(client_socket, "An internal error occurred while processing the command.");
    }
  }
    buffer.clear();
}
