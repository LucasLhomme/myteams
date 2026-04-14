/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** subscription commands
*/

#include "server/commands/cmd_subscribe.hpp"
#include "server/commands/cmd_subscribed.hpp"
#include "server/commands/cmd_unsubscribe.hpp"
#include "server/server.hpp"
#include <uuid/uuid.h>
#include <string>

extern "C" {
    #include "../../../libs/myteams/logging_server.h"
    #include "../../../libs/myteams/logging_client.h"
}

static std::string uuid_to_string(const uuid_t &uuid)
{
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}

static User* get_user_from_socket(Server &server, int client_socket)
{
    auto &socket_to_uuid = server.get_socket_to_uuid();
    auto user_it = socket_to_uuid.find(client_socket);
    if (user_it == socket_to_uuid.end())
        return nullptr;

    std::string user_uuid = user_it->second;
    for (auto u : server.get_users()) {
        if (uuid_to_string(u->uuid) == user_uuid)
            return u;
    }
    return nullptr;
}

void CmdSubscribe::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        return;
    }
    // on recup le user 
    User *user = get_user_from_socket(server, client_socket);
    if (!user) {
        server.send_response(client_socket, "ERROR: Not connected");
        return;
    }
    // on ajoute l'uuid de la team au vecteur de l'utilisateur
    std::string team_uuid_arg = args[1];
    user->subscribed_teams.push_back(team_uuid_arg);

    // et on ajoute les chanel de la team aussi
    for (auto channel : server.get_channels()) {
        if (uuid_to_string(channel->team_uuid) == team_uuid_arg) {
            user->subscribed_teams.push_back(uuid_to_string(channel->uuid));
        }
    }

    // Log the subscription event
    server_event_user_subscribed(team_uuid_arg.c_str(), uuid_to_string(user->uuid).c_str());
    
    std::string response = uuid_to_string(user->uuid) + " " + team_uuid_arg;
    server.send_response(client_socket, response);
}

void CmdSubscribed::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    // on recup le user
    User *user = get_user_from_socket(server, client_socket);
    if (!user) {
        return;
    }

    // list all team if no teams asked
    if (args.size() < 2) {
        std::string response = "";
        for (const auto &team_uuid_str : user->subscribed_teams) {
            for (auto team : server.get_teams()) {
                if (uuid_to_string(team->uuid) == team_uuid_str) {
                    response += "TEAM|" + team_uuid_str + "|" + team->name + "|" + team->description + "\n";
                    break;
                }
            }
        }
        server.send_response(client_socket, response.empty() ? "NO_TEAMS" : response);
        return;
    }

    // and then list users subscribed to the team
    std::string team_uuid_arg = args[1];
    std::string response = "";
    for (auto u : server.get_users()) {
        for (const auto &subscribed_uuid_str : u->subscribed_teams) {
            if (subscribed_uuid_str == team_uuid_arg) {
                int status = 1;
                response += "USER|" + uuid_to_string(u->uuid) + "|" + u->username + "|" + std::to_string(status) + "\n";
                break;
            }
        }
    }
    server.send_response(client_socket, response.empty() ? "NO_USERS" : response);
}

void CmdUnsubscribe::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    if (args.size() < 2) {
        return;
    }

    // on recup le user
    User *user = get_user_from_socket(server, client_socket);
    if (!user) {
        server.send_response(client_socket, "ERROR: Not connected");
        return;
    }

    // on suppr l'uuid de la team du vecteur de l'utilisateur, et aussi les channels de la team
    std::string team_uuid_arg = args[1];
    auto it = user->subscribed_teams.begin();
    while (it != user->subscribed_teams.end()) {
        if (*it == team_uuid_arg) {
            it = user->subscribed_teams.erase(it);
            continue;
        }
        bool is_channel_of_team = false;
        for (auto channel : server.get_channels()) {
            if (uuid_to_string(channel->team_uuid) == team_uuid_arg && 
                uuid_to_string(channel->uuid) == *it) {
                is_channel_of_team = true;
                break;
            }
        }
        if (is_channel_of_team)
            it = user->subscribed_teams.erase(it);
        else
            ++it;
    }

    // Log the unsubscription event
    server_event_user_unsubscribed(team_uuid_arg.c_str(), uuid_to_string(user->uuid).c_str());
    
    std::string response = uuid_to_string(user->uuid) + " " + team_uuid_arg;
    server.send_response(client_socket, response);
}
