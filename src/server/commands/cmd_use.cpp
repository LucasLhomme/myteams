/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_use
*/

#include "server/commands/cmd_use.hpp"
#include "server/server.hpp"
#include <uuid/uuid.h>

static bool is_valid_uuid(const std::string &arg)
{
    uuid_t uuid;

    return uuid_parse(arg.c_str(), uuid) == 0;
}

static std::string uuid_to_string(const uuid_t &uuid)
{
    char out[MAX_UUID_LENGTH];

    uuid_unparse(uuid, out);
    return std::string(out);
}

static bool team_exists(const Server &server, const std::string &team_uuid)
{
    for (const Team *team : server.get_teams()) {
        if (uuid_to_string(team->uuid) == team_uuid) {
            return true;
        }
    }
    return false;
}

static bool channel_exists_in_team(const Server &server, const std::string &team_uuid,
    const std::string &channel_uuid)
{
    for (const Channel *channel : server.get_channels()) {
        if (uuid_to_string(channel->uuid) == channel_uuid &&
            uuid_to_string(channel->team_uuid) == team_uuid) {
            return true;
        }
    }
    return false;
}

static bool thread_exists_in_channel(const Server &server, const std::string &team_uuid,
    const std::string &channel_uuid, const std::string &thread_uuid)
{
    for (const Thread *thread : server.get_threads()) {
        if (uuid_to_string(thread->uuid) == thread_uuid &&
            uuid_to_string(thread->team_uuid) == team_uuid &&
            uuid_to_string(thread->channel_uuid) == channel_uuid) {
            return true;
        }
    }
    return false;
}

void CmdUse::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    auto &current_team = server.get_use_team_context();
    auto &current_channel = server.get_use_channel_context();
    auto &current_thread = server.get_use_thread_context();

    if (args.size() > 4) {
        server.send_response(client_socket,
            "Usage: /use [team_uuid] [channel_uuid] [thread_uuid]");
        return;
    }

    if (args.size() == 1) {
        current_team.erase(client_socket);
        current_channel.erase(client_socket);
        current_thread.erase(client_socket);
        server.send_response(client_socket, "Use context cleared");
        return;
    }

    const std::string &team_uuid = args[1];

    if (!is_valid_uuid(team_uuid) || !team_exists(server, team_uuid)) {
        server.send_response(client_socket, "Unknown team_uuid");
        return;
    }
    current_team[client_socket] = team_uuid;
    current_channel[client_socket].clear();
    current_thread[client_socket].clear();

    if (args.size() == 2) {
        server.send_response(client_socket, "Team context set");
        return;
    }

    const std::string &channel_uuid = args[2];

    if (!is_valid_uuid(channel_uuid) || !channel_exists_in_team(server, team_uuid, channel_uuid)) {
        server.send_response(client_socket, "Unknown channel_uuid for current team");
        return;
    }
    current_channel[client_socket] = channel_uuid;

    if (args.size() == 3) {
        server.send_response(client_socket, "Channel context set");
        return;
    }

    const std::string &thread_uuid = args[3];

    if (!is_valid_uuid(thread_uuid) ||
        !thread_exists_in_channel(server, team_uuid, channel_uuid, thread_uuid)) {
        server.send_response(client_socket,
            "Unknown thread_uuid for current team/channel");
        return;
    }
    current_thread[client_socket] = thread_uuid;
    server.send_response(client_socket, "Thread context set");
}