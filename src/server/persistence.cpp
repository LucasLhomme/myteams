/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** persistence management
*/

#include "server/server.hpp"
#include <fstream>
#include <cstring>
#include <iostream>

extern "C" {
#include "../../libs/myteams/logging_server.h"
}

void Server::save_state() {
  // TODO: remplacer par un json
  std::ofstream file("server_save.txt");

  if (!file.is_open())
    return;

  for (const auto &user : _users) {
    char uuid_str[37];
    uuid_unparse(user->uuid, uuid_str);
    file << "USER " << uuid_str << " " << user->username << std::endl;
  }

  for (const auto &team : _teams) {
    char uuid_str[37];
    uuid_unparse(team->uuid, uuid_str);
    file << "TEAM " << uuid_str << " " << team->name << " \"" << team->description << "\"" << std::endl;
  }

  file.close();
}

void Server::load_state() {
  // TODO: implement
  return;
}
