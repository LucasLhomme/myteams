/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_create
*/

#include "server/commands/cmd_create.hpp"
#include "server/server.hpp"
#include "server/team.hpp"
#include <algorithm>
#include <cstring>
#include <uuid/uuid.h>
#include <ctime>
#include <iostream>
#include <sstream>

extern "C" {
    #include "../../../libs/myteams/logging_server.h"
}

static std::string get_user_uuid(Server &server, int client_socket)
{
    const auto &socket_to_uuid = server.get_socket_to_uuid();
    const auto it = socket_to_uuid.find(client_socket);

    if (it == socket_to_uuid.end()) {
        return "";
    }
    return it->second;
}

static bool parse_uuid_or_fail(const std::string &uuid_str, uuid_t out)
{
    return uuid_parse(uuid_str.c_str(), out) == 0;
}

static std::string uuid_to_string(const uuid_t uuid)
{
    char uuid_str[MAX_UUID_LENGTH];

    uuid_unparse(uuid, uuid_str);
    return std::string(uuid_str);
}

static bool has_ctx(const std::unordered_map<int, std::string> &ctx, int client_socket)
{
    const auto it = ctx.find(client_socket);

    return it != ctx.end() && !it->second.empty();
}

static bool check_team_channel_fields(const std::vector<std::string> &args)
{
    return args.size() == 3 &&
        args[1].length() < MAX_NAME_LENGTH &&
        args[2].length() < MAX_DESCRIPTION_LENGTH;
}

static bool check_thread_fields(const std::vector<std::string> &args)
{
    return args.size() == 3 &&
        args[1].length() < MAX_NAME_LENGTH &&
        args[2].length() < MAX_BODY_LENGTH;
}

static bool check_reply_field(const std::vector<std::string> &args)
{
    return args.size() == 2 && args[1].length() < MAX_BODY_LENGTH;
}

static void send_event_to_user(Server &server, const std::string &user_uuid,
    const std::string &payload)
{
    auto &socket_to_uuid = server.get_socket_to_uuid();

    for (const auto &entry : socket_to_uuid) {
        if (entry.second == user_uuid) {
            server.send_response(entry.first, payload);
        }
    }
}

static void send_event_to_all_logged_users(Server &server, const std::string &payload)
{
    auto &socket_to_uuid = server.get_socket_to_uuid();

    for (const auto &entry : socket_to_uuid) {
        server.send_response(entry.first, payload);
    }
}

static void send_event_to_team_subscribers(Server &server, const std::string &team_uuid,
    const std::string &payload)
{
    auto &subscriptions = server.get_subscriptions_by_user();

    for (const auto &entry : subscriptions) {
        const auto &user_uuid = entry.first;
        const auto &subs = entry.second;

        if (std::find(subs.begin(), subs.end(), team_uuid) != subs.end()) {
            send_event_to_user(server, user_uuid, payload);
        }
    }
}

static std::string make_event_payload(const std::string &type,
    const std::vector<std::string> &fields)
{
    std::ostringstream payload;

    payload << "EVENT\t" << type;
    for (const auto &field : fields) {
        payload << '\t' << field;
    }
    return payload.str();
}

static bool is_user_subscribed(Server &server, const std::string &user_uuid, const std::string &team_uuid)
{
    const auto &subscriptions = server.get_subscriptions_by_user();
    auto it = subscriptions.find(user_uuid);
    if (it == subscriptions.end())
        return false;
    return std::find(it->second.begin(), it->second.end(), team_uuid) != it->second.end();
}

static void create_team(Server &server, int client_socket, const std::vector<std::string> &args) {
    Team *team = new Team;
    uuid_generate(team->uuid);
    std::strncpy(team->name, args[1].c_str(), MAX_NAME_LENGTH);
    std::strncpy(team->description, args[2].c_str(), MAX_DESCRIPTION_LENGTH);
    team->name[MAX_NAME_LENGTH - 1] = '\0';
    team->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';
    server.get_teams().push_back(team);
    char u_str[MAX_UUID_LENGTH];
    uuid_unparse(team->uuid, u_str);
    std::string user_uuid = get_user_uuid(server, client_socket);
    server_event_team_created(u_str, team->name, user_uuid.c_str());
    server.send_response(client_socket, "Team created with UUID: " + std::string(u_str));
    send_event_to_all_logged_users(server, make_event_payload("TEAM_CREATED",
        {u_str, team->name, team->description}));
}

static void create_channel(Server &server, int client_socket, const std::vector<std::string> &args,
    const std::string &team_uuid_str) {
    std::string user_uuid = get_user_uuid(server, client_socket);
    if (!is_user_subscribed(server, user_uuid, team_uuid_str)) {
        std::cout << "create blocked due to the non subscription of the user" << std::endl;
        server.send_response(client_socket, "ERROR: Unauthorized. You can't create a channel if you're not subscribed to the team");
        return;
    }
    Channel *channel = new Channel;

    if (!parse_uuid_or_fail(team_uuid_str, channel->team_uuid)) {
        delete channel;
        server.send_response(client_socket, "ERROR: Invalid team context, use /use again");
        return;
    }
    uuid_generate(channel->uuid);
    std::strncpy(channel->channel_name, args[1].c_str(), MAX_NAME_LENGTH);
    std::strncpy(channel->channel_description, args[2].c_str(), MAX_DESCRIPTION_LENGTH);
    channel->channel_name[MAX_NAME_LENGTH - 1] = '\0';
    channel->channel_description[MAX_DESCRIPTION_LENGTH - 1] = '\0';
    server.get_channels().push_back(channel);
    char channel_uuid[MAX_UUID_LENGTH];
    uuid_unparse(channel->uuid, channel_uuid);
    server_event_channel_created(team_uuid_str.c_str(), channel_uuid, channel->channel_name);
    server.send_response(client_socket, "Channel created with UUID: " + std::string(channel_uuid));
    send_event_to_team_subscribers(server, team_uuid_str, make_event_payload("CHANNEL_CREATED",
        {channel_uuid, channel->channel_name, channel->channel_description}));
}

static void create_thread(Server &server, int client_socket, const std::vector<std::string> &args,
    const std::string &team_uuid_str, const std::string &channel_uuid_str) {
    std::string user_uuid = get_user_uuid(server, client_socket);
    if (!is_user_subscribed(server, user_uuid, team_uuid_str)) {
        std::cout << "create blocked due to the non subscription of the user" << std::endl;
        server.send_response(client_socket, "ERROR: Unauthorized. You can't create a thread if you're not subscribed to the team");
        return;
    }
    Thread *thread = new Thread;

    if (!parse_uuid_or_fail(team_uuid_str, thread->team_uuid) ||
        !parse_uuid_or_fail(channel_uuid_str, thread->channel_uuid)) {
        delete thread;
        server.send_response(client_socket, "ERROR: Invalid team/channel context, use /use again");
        return;
    }
    uuid_generate(thread->uuid);
    thread->timestamp = time(nullptr);
    std::strncpy(thread->creator_uuid, user_uuid.c_str(), MAX_UUID_LENGTH - 1);
    thread->creator_uuid[MAX_UUID_LENGTH - 1] = '\0';
    std::strncpy(thread->thread_title, args[1].c_str(), MAX_NAME_LENGTH - 1);
    std::strncpy(thread->thread_message, args[2].c_str(), MAX_BODY_LENGTH - 1);
    thread->thread_title[MAX_NAME_LENGTH - 1] = '\0';
    thread->thread_message[MAX_BODY_LENGTH - 1] = '\0';
    server.get_threads().push_back(thread);
    char thread_uuid[MAX_UUID_LENGTH];
    uuid_unparse(thread->uuid, thread_uuid);
    server_event_thread_created(channel_uuid_str.c_str(), thread_uuid, user_uuid.c_str(),
        thread->thread_title, thread->thread_message);
    server.send_response(client_socket, "Thread created with UUID: " + std::string(thread_uuid) + 
        " " + user_uuid + " " + std::to_string(thread->timestamp));
    send_event_to_team_subscribers(server, team_uuid_str, make_event_payload("THREAD_CREATED",
        {thread_uuid, user_uuid, std::to_string(thread->timestamp),
        thread->thread_title, thread->thread_message}));
}

static void create_reply(Server &server, int client_socket, const std::vector<std::string> &args,
    const std::string &thread_uuid_str) {
    std::string user_uuid = get_user_uuid(server, client_socket);
    std::string team_uuid_str;
    bool thread_exists = false;
    for (const auto *thread : server.get_threads()) {
        if (thread != nullptr && uuid_to_string(thread->uuid) == thread_uuid_str) {
            team_uuid_str = uuid_to_string(thread->team_uuid);
            thread_exists = true;
            break;
        }
    }
    if (!thread_exists) {
        server.send_response(client_socket, "ERROR: UNKNOWN_THREAD:" + thread_uuid_str);
        return;
    }
    if (!team_uuid_str.empty() && !is_user_subscribed(server, user_uuid, team_uuid_str)) {
        std::cout << "create blocked due to the non subscription of the user" << std::endl;
        server.send_response(client_socket, "ERROR: Unauthorized. You can't create a reply if you're not subscribed to the team");
        return;
    }
    Reply *reply = new Reply;

    if (!parse_uuid_or_fail(thread_uuid_str, reply->thread_uuid)) {
        delete reply;
        server.send_response(client_socket, "ERROR: Invalid thread context, use /use again");
        return;
    }
    uuid_generate(reply->uuid);
    reply->timestamp = time(nullptr);
    std::strncpy(reply->creator_uuid, user_uuid.c_str(), MAX_UUID_LENGTH - 1);
    reply->creator_uuid[MAX_UUID_LENGTH - 1] = '\0';
    std::strncpy(reply->reply_message, args[1].c_str(), MAX_BODY_LENGTH - 1);
    reply->reply_message[MAX_BODY_LENGTH - 1] = '\0';
    server.get_replies().push_back(reply);
    
    char reply_uuid[MAX_UUID_LENGTH];
    uuid_unparse(reply->uuid, reply_uuid);
    server_event_reply_created(thread_uuid_str.c_str(), user_uuid.c_str(), reply->reply_message);
    server.send_response(client_socket, "Reply created with UUID: " + std::string(reply_uuid) + 
        " " + user_uuid + " " + std::to_string(reply->timestamp));

    if (!team_uuid_str.empty()) {
        send_event_to_team_subscribers(server, team_uuid_str,
            make_event_payload("THREAD_REPLY_RECEIVED",
            {team_uuid_str, thread_uuid_str, user_uuid, reply->reply_message}));
    }
}

void CmdCreate::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{
    try {
    auto &current_team = server.get_use_team_context();
    auto &current_channel = server.get_use_channel_context();
    auto &current_thread = server.get_use_thread_context();
    const bool has_team = has_ctx(current_team, client_socket);
    const bool has_channel = has_ctx(current_channel, client_socket);
    const bool has_thread = has_ctx(current_thread, client_socket);

    if (args.size() != 2 && args.size() != 3) {
        server.send_response(client_socket,
            "Usage: /create [name] [description] or /create [reply_body]");
        return;
    }

    if (args.size() == 3) {
        if (!has_team) {
            if (!check_team_channel_fields(args)) {
                server.send_response(client_socket, "ERROR: Invalid team name/description length");
                return;
            }
            create_team(server, client_socket, args);
        } else if (!has_channel) {
            if (!check_team_channel_fields(args)) {
                server.send_response(client_socket, "ERROR: Invalid channel name/description length");
                return;
            }
            create_channel(server, client_socket, args, current_team[client_socket]);
        } else {
            if (!check_thread_fields(args)) {
                server.send_response(client_socket, "ERROR: Invalid thread title/message length");
                return;
            }
            create_thread(server, client_socket, args, current_team[client_socket],
                current_channel[client_socket]);
        }
    }
    if (args.size() == 2) {
        if (!has_thread) {
            server.send_response(client_socket,
                "ERROR: Reply creation requires /use team_uuid channel_uuid thread_uuid");
            return;
        }
        if (!check_reply_field(args)) {
            server.send_response(client_socket, "ERROR: Invalid reply body length");
            return;
        }
        create_reply(server, client_socket, args, current_thread[client_socket]);
    }
    server.save_state();

    } catch (const std::exception &e) {
        std::cout << "[ERROR] " << e.what() << std::endl;
    }}