/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_list
*/

#include "server/commands/cmd_list.hpp"
#include "server/server.hpp"
#include <sstream>

extern "C" {
    #include "../../../libs/myteams/logging_server.h"
}

static bool has_ctx(const std::unordered_map<int, std::string> &ctx, int client_socket)
{
    const auto it = ctx.find(client_socket);

    return it != ctx.end() && !it->second.empty();
}

static std::string uuid_to_string(const uuid_t uuid)
{
    char uuid_str[MAX_UUID_LENGTH];

    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}

static void append_team_list(Server &server, std::ostringstream &response)
{
    const auto &teams = server.get_teams();

    for (const auto *team : teams) {
        if (team == nullptr) {
            continue;
        }
        response << "TEAM\t" << uuid_to_string(team->uuid) << '\t' << team->name << '\t'
            << team->description << '\n';
    }
}

static void append_channel_list(Server &server, std::ostringstream &response,
    const std::string &team_uuid)
{
    const auto &channels = server.get_channels();

    for (const auto *channel : channels) {
        if (channel == nullptr || uuid_to_string(channel->team_uuid) != team_uuid) {
            continue;
        }
        response << "CHANNEL\t" << uuid_to_string(channel->uuid) << '\t'
            << channel->channel_name << '\t' << channel->channel_description << '\n';
    }
}

static void append_thread_list(Server &server, std::ostringstream &response,
    const std::string &channel_uuid)
{
    const auto &threads = server.get_threads();

    for (const auto *thread : threads) {
        if (thread == nullptr || uuid_to_string(thread->channel_uuid) != channel_uuid) {
            continue;
        }
        response << "THREAD\t" << uuid_to_string(thread->uuid) << '\t'
            << thread->creator_uuid << '\t' << thread->timestamp << '\t'
            << thread->thread_title << '\t' << thread->thread_message << '\n';
    }
}

static void append_reply_list(Server &server, std::ostringstream &response,
    const std::string &thread_uuid)
{
    const auto &replies = server.get_replies();

    for (const auto *reply : replies) {
        if (reply == nullptr || uuid_to_string(reply->thread_uuid) != thread_uuid) {
            continue;
        }
        response << "REPLY\t" << uuid_to_string(reply->thread_uuid) << '\t'
            << reply->creator_uuid << '\t' << reply->timestamp << '\t'
            << reply->reply_message << '\n';
    }
}

void CmdList::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    auto &current_team = server.get_use_team_context();
    auto &current_channel = server.get_use_channel_context();
    auto &current_thread = server.get_use_thread_context();
    const bool has_team = has_ctx(current_team, client_socket);
    const bool has_channel = has_ctx(current_channel, client_socket);
    const bool has_thread = has_ctx(current_thread, client_socket);

    if (args.size() != 1) {
        server.send_response(client_socket, "Usage: /list");
        return;
    }

    std::ostringstream response;

    if (!has_team) {
        append_team_list(server, response);
    } else if (!has_channel) {
        append_channel_list(server, response, current_team[client_socket]);
    } else if (!has_thread) {
        append_thread_list(server, response, current_channel[client_socket]);
    } else {
        append_reply_list(server, response, current_thread[client_socket]);
    }

    std::string output = response.str();
    if (output.empty()) {
        output = "No items available";
    }
    server.send_response(client_socket, output);
}