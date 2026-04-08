/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** logout command
*/

#include "server/commands/cmd_logout.hpp"
#include "server/server.hpp"

#include <cstring>
extern "C" {
#include "../../../libs/myteams/logging_server.h"
}

void CmdLogout::execute(Server &server, int client_socket, const std::vector<std::string> &args) {
  (void)args;
  auto it = server.get_socket_to_uuid().find(client_socket);
  if (it != server.get_socket_to_uuid().end()) {
    std::string uuid_str = it->second;
    std::string username_str = "";

    for (auto &user : server.get_users()) {
      char u_str[MAX_UUID_LENGTH];
      uuid_unparse(user->uuid, u_str);
      if (std::string(u_str) == uuid_str) {
        username_str = user->username;
        user->is_online = false;
        break;
      }
    }
    
    // IMPORTANT: Send user info BEFORE removing from maps
    // Format: LOGOUT_DATA <uuid> <username>
    // This way client receives the data with certainty before connection closes
    server.send_response(client_socket, "LOGOUT_DATA " + uuid_str + " " + username_str);
    
    // NOW we can clean up the connection data
    server.save_state();
    server_event_user_logged_out(uuid_str.c_str());
    server.get_socket_to_uuid().erase(it);
    server.get_socket_to_name().erase(client_socket);
  } else {
    server.send_response(client_socket, "Error: Not logged in");
  }
}
