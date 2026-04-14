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
#include "exception.hpp"
#include "server/server_exception.hpp"
#include <uuid/uuid.h>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <exception>

extern "C" {
    #include "../../../libs/myteams/logging_server.h"
}

static std::string uuid_to_string(const uuid_t &uuid)
{
    char uuid_str[MAX_UUID_LENGTH];
    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}

static std::string get_user_uuid_from_socket(Server &server, int client_socket)
{
    auto &socket_to_uuid = server.get_socket_to_uuid();
    auto user_it = socket_to_uuid.find(client_socket);
    if (user_it == socket_to_uuid.end())
        return "";
    return user_it->second;
}

static Team *get_team_by_uuid(Server &server, const std::string &team_uuid)
{
    for (auto *team : server.get_teams()) {
        if (team != nullptr && uuid_to_string(team->uuid) == team_uuid) {
            return team;
        }
    }
    return nullptr;
}

static std::unordered_map<std::string, std::vector<std::string>> g_subscriptions_by_user;

void CmdSubscribe::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    try {
        if (args.size() < 2) {
            throw SubscriptionException("Usage: /subscribe [team_uuid]");
        }

        const std::string user_uuid = get_user_uuid_from_socket(server, client_socket);
        if (user_uuid.empty()) {
            throw SubscriptionException("ERROR: Not connected");
        }

        std::string team_uuid_arg = args[1];
        if (get_team_by_uuid(server, team_uuid_arg) == nullptr) {
            throw SubscriptionException("ERROR_UNKNOWN_TEAM: " + team_uuid_arg);
        }

        auto &subs = g_subscriptions_by_user[user_uuid];

        if (std::find(subs.begin(), subs.end(), team_uuid_arg) == subs.end()) {
            subs.push_back(team_uuid_arg);
        }

        for (auto *channel : server.get_channels()) {
            if (channel != nullptr && uuid_to_string(channel->team_uuid) == team_uuid_arg) {
                const std::string channel_uuid = uuid_to_string(channel->uuid);
                if (std::find(subs.begin(), subs.end(), channel_uuid) == subs.end()) {
                    subs.push_back(channel_uuid);
                }
            }
        }

        server_event_user_subscribed(team_uuid_arg.c_str(), user_uuid.c_str());

        std::string response = user_uuid + " " + team_uuid_arg;
        server.send_response(client_socket, response);
    } catch (const SubscriptionException &e) {
        server.send_response(client_socket, e.what());
    } catch (const std::exception &e) {
        server.send_response(client_socket, "ERROR_SUBSCRIBE_EXCEPTION: " + std::string(e.what()));
    } catch (...) {
        server.send_response(client_socket, "ERROR_SUBSCRIBE_EXCEPTION: unknown");
    }
}

void CmdSubscribed::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    try {
        const std::string requester_uuid = get_user_uuid_from_socket(server, client_socket);
        if (requester_uuid.empty()) {
            throw SubscriptionException("ERROR: Not connected");
        }

        if (args.size() < 2) {
            std::string response = "";
            const auto it = g_subscriptions_by_user.find(requester_uuid);
            const std::vector<std::string> empty;
            const std::vector<std::string> &subs =
                (it != g_subscriptions_by_user.end()) ? it->second : empty;

            for (const auto &team_uuid_str : subs) {
                for (auto team : server.get_teams()) {
                    if (team != nullptr && uuid_to_string(team->uuid) == team_uuid_str) {
                        response += "TEAM|" + team_uuid_str + "|" + team->name + "|" +
                            team->description + "\n";
                        break;
                    }
                }
            }
            server.send_response(client_socket, response.empty() ? "NO_TEAMS" : response);
            return;
        }

        std::string team_uuid_arg = args[1];
        std::string response = "";
        for (auto u : server.get_users()) {
            if (u == nullptr)
                continue;
            const std::string user_uuid = uuid_to_string(u->uuid);
            const auto it = g_subscriptions_by_user.find(user_uuid);
            if (it == g_subscriptions_by_user.end())
                continue;
            const auto &subs = it->second;
            if (std::find(subs.begin(), subs.end(), team_uuid_arg) != subs.end()) {
                int status = 1;
                response += "USER|" + user_uuid + "|" + u->username + "|" +
                    std::to_string(status) + "\n";
            }
        }
        server.send_response(client_socket, response.empty() ? "NO_USERS" : response);
    } catch (const SubscriptionException &e) {
        server.send_response(client_socket, e.what());
    } catch (const std::exception &e) {
        server.send_response(client_socket, "ERROR_SUBSCRIBED_EXCEPTION: " + std::string(e.what()));
    } catch (...) {
        server.send_response(client_socket, "ERROR_SUBSCRIBED_EXCEPTION: unknown");
    }
}

void CmdUnsubscribe::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    try {
        if (args.size() < 2) {
            throw SubscriptionException("Usage: /unsubscribe [team_uuid]");
        }

        const std::string user_uuid = get_user_uuid_from_socket(server, client_socket);
        if (user_uuid.empty()) {
            throw SubscriptionException("ERROR: Not connected");
        }

        auto &subs = g_subscriptions_by_user[user_uuid];
        std::string team_uuid_arg = args[1];
        auto it = subs.begin();
        while (it != subs.end()) {
            if (*it == team_uuid_arg) {
                it = subs.erase(it);
                continue;
            }
            bool is_channel_of_team = false;
            for (auto *channel : server.get_channels()) {
                if (channel != nullptr &&
                    uuid_to_string(channel->team_uuid) == team_uuid_arg &&
                    uuid_to_string(channel->uuid) == *it) {
                    is_channel_of_team = true;
                    break;
                }
            }
            if (is_channel_of_team)
                it = subs.erase(it);
            else
                ++it;
        }

        server_event_user_unsubscribed(team_uuid_arg.c_str(), user_uuid.c_str());

        std::string response = user_uuid + " " + team_uuid_arg;
        server.send_response(client_socket, response);
    } catch (const SubscriptionException &e) {
        server.send_response(client_socket, e.what());
    } catch (const std::exception &e) {
        server.send_response(client_socket, "ERROR_UNSUBSCRIBE_EXCEPTION: " + std::string(e.what()));
    } catch (...) {
        server.send_response(client_socket, "ERROR_UNSUBSCRIBE_EXCEPTION: unknown");
    }
}
