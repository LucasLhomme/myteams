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
    if (user == nullptr) {
      continue;
    }
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(user->uuid, uuid_str);
    file << "USER " << uuid_str << " " << user->username << std::endl;
  }

  for (const auto &team : _teams) {
    if (team == nullptr) {
      continue;
    }
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(team->uuid, uuid_str);
    file << "TEAM " << uuid_str << " " << team->name << " \"" << team->description << "\"" << std::endl;
  }

  for (const auto &channel : _channels) {
    if (channel == nullptr) {
      continue;
    }
    char channel_uuid[MAX_UUID_LENGTH];
    char team_uuid[MAX_UUID_LENGTH];
    uuid_unparse(channel->uuid, channel_uuid);
    uuid_unparse(channel->team_uuid, team_uuid);
    file << "CHANNEL " << team_uuid << " " << channel_uuid << " " << channel->channel_name << " \"" << channel->channel_description << "\"" << std::endl;
  }

  for (const auto &thread : _threads) {
    if (thread == nullptr) {
      continue;
    }
    char thread_uuid[MAX_UUID_LENGTH];
    char team_uuid[MAX_UUID_LENGTH];
    char channel_uuid[MAX_UUID_LENGTH];
    uuid_unparse(thread->uuid, thread_uuid);
    uuid_unparse(thread->team_uuid, team_uuid);
    uuid_unparse(thread->channel_uuid, channel_uuid);
    file << "THREAD " << team_uuid << " " << channel_uuid << " " << thread_uuid << " " << thread->thread_title << " \"" << thread->thread_message << "\"" << std::endl;
  }

  for (const auto &reply : _replies) {
    if (reply == nullptr) {
      continue;
    }
    char reply_uuid[MAX_UUID_LENGTH];
    char thread_uuid[MAX_UUID_LENGTH];
    uuid_unparse(reply->uuid, reply_uuid);
    uuid_unparse(reply->thread_uuid, thread_uuid);
    file << "REPLY " << thread_uuid << " " << reply_uuid << " \"" << reply->reply_message << "\"" << std::endl;
  }

  file.close();
}

void Server::load_state() {
  // TODO: implement
  return;
}
