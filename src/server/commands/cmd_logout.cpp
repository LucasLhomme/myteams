/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** logout command
*/

#include "server/server.hpp"

#include <cstring>
extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void Server::cmd_logout(int client_socket, const std::vector<std::string> &args) {
  (void)args;
  auto it = _socket_to_uuid.find(client_socket);
  if (it != _socket_to_uuid.end()) {
    std::string uuid_str = it->second;

    for (auto &user : _users) {
      char u_str[MAX_UUID_LENGTH];
      uuid_unparse(user->uuid, u_str);
      if (std::string(u_str) == uuid_str) {
        user->is_online = false;
        break;
      }
    }

    server_event_user_logged_out(uuid_str.c_str());
    _socket_to_uuid.erase(it);
    _socket_to_name.erase(client_socket);
  }
  send_response(client_socket, "Logout success");
  remove_client(client_socket);
}
