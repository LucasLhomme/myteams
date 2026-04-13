/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_info
*/

#include "server/commands/cmd_info.hpp"
#include "server/server.hpp"

#include <sstream>

static std::string uuid_to_string(const uuid_t uuid)
{
    char uuid_str[MAX_UUID_LENGTH];

    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}

static bool append_user_details(Server &server, std::ostringstream &response,
    const std::string &user_uuid)
{
    const auto &users = server.get_users();

    for (const auto *user : users) {
        if (user == nullptr || uuid_to_string(user->uuid) != user_uuid) {
            continue;
        }
        response << "USER\t" << uuid_to_string(user->uuid) << '\t' << user->username << '\t'
            << (user->is_online ? 1 : 0);
        return true;
    }
    return false;
}

static bool append_team_details(Server &server, std::ostringstream &response,
    const std::string &team_uuid)
{
    const auto &teams = server.get_teams();

    for (const auto *team : teams) {
        if (team == nullptr || uuid_to_string(team->uuid) != team_uuid) {
            continue;
        }
        response << "TEAM\t" << uuid_to_string(team->uuid) << '\t' << team->name << '\t'
            << team->description;
        return true;
    }
    return false;
}

static bool append_channel_details(Server &server, std::ostringstream &response,
    const std::string &channel_uuid)
{
    const auto &channels = server.get_channels();

    for (const auto *channel : channels) {
        if (channel == nullptr || uuid_to_string(channel->uuid) != channel_uuid) {
            continue;
        }
        response << "CHANNEL\t" << uuid_to_string(channel->uuid) << '\t'
            << channel->channel_name << '\t' << channel->channel_description;
        return true;
    }
    return false;
}

static bool append_thread_details(Server &server, std::ostringstream &response,
    const std::string &thread_uuid)
{
    const auto &threads = server.get_threads();

    for (const auto *thread : threads) {
        if (thread == nullptr || uuid_to_string(thread->uuid) != thread_uuid) {
            continue;
        }
        response << "THREAD\t" << uuid_to_string(thread->uuid) << '\t'
            << thread->creator_uuid << '\t' << thread->timestamp << '\t'
            << thread->thread_title << '\t' << thread->thread_message;
        return true;
    }
    return false;
}

void CmdInfo::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() != 1) {
        server.send_response(client_socket, "Usage: /info");
        return;
    }

    const auto &current_team = server.get_use_team_context();
    const auto &current_channel = server.get_use_channel_context();
    const auto &current_thread = server.get_use_thread_context();
    const auto &socket_to_uuid = server.get_socket_to_uuid();
    std::ostringstream response;

    if (current_team.find(client_socket) == current_team.end()) {
        const auto it = socket_to_uuid.find(client_socket);
        if (it == socket_to_uuid.end()) {
            server.send_response(client_socket, "ERROR");
            return;
        }
        if (!append_user_details(server, response, it->second)) {
            server.send_response(client_socket, "ERROR_UNKNOWN_USER:" + it->second);
            return;
        }
    } else if (current_channel.find(client_socket) == current_channel.end()) {
        const std::string &team_uuid = current_team.at(client_socket);
        if (!append_team_details(server, response, team_uuid)) {
            server.send_response(client_socket, "ERROR_UNKNOWN_TEAM:" + team_uuid);
            return;
        }
    } else if (current_thread.find(client_socket) == current_thread.end()) {
        const std::string &channel_uuid = current_channel.at(client_socket);
        if (!append_channel_details(server, response, channel_uuid)) {
            server.send_response(client_socket, "ERROR_UNKNOWN_CHANNEL:" + channel_uuid);
            return;
        }
    } else {
        const std::string &thread_uuid = current_thread.at(client_socket);
        if (!append_thread_details(server, response, thread_uuid)) {
            server.send_response(client_socket, "ERROR_UNKNOWN_THREAD:" + thread_uuid);
            return;
        }
    }

    server.send_response(client_socket, response.str());
}