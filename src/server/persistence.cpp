/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** JSON persistence — save & load server state
*/

#include "server/server.hpp"
#include <fstream>
#include <sstream>
#include <cstring>
#include <iostream>
#include <stdexcept>

extern "C" {
#include "../../libs/myteams/logging_server.h"
}

//fonction qui escape les caracteres speciaux du json
static std::string json_escape(const std::string &s)
{
    std::string out;

    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\"";
                break;
            case '\\': out += "\\\\";
                break;
            case '\n': out += "\\n";
                break;
            case '\r': out += "\\r";
                break;
            case '\t': out += "\\t";
                break;
            default:   out += c;
                break;
        }
    }
    return out;
}

//fonction qui convertit un uuid en string
static std::string uuid_to_str(const uuid_t u)
{
    char buf[MAX_UUID_LENGTH];

    uuid_unparse(u, buf);
    return std::string(buf);
}


//c la fonction qui va ecrire dans le fichier json
void Server::save_state()
{
    std::ofstream file("server_save.json");
    if (!file.is_open()) {
        std::cout << "[ERROR] Cannot open server_save.json for writing" << std::endl;
        return;
    }

    //je write les infos des users dans le json
    file << "{\n";
    file << "  \"users\": [\n";

    bool first = true;
    for (const auto &user : _users) {
        if (!user)
            continue;
        if (!first)
            file << ",\n";
        first = false;
        file << "    {\n";
        file << "      \"uuid\": \""     << json_escape(uuid_to_str(user->uuid))    << "\",\n";
        file << "      \"username\": \"" << json_escape(user->username)              << "\",\n";
        file << "      \"is_online\": "  << (user->is_online ? "true" : "false")    << "\n";
        file << "    }";
    }

    //je write les infos des teams dans le json
    file << "\n  ],\n";
    file << "  \"teams\": [\n";

    first = true;
    for (const auto &team : _teams) {
        if (!team)
            continue;
        if (!first)
            file << ",\n";
        first = false;
        file << "    {\n";
        file << "      \"uuid\": \""        << json_escape(uuid_to_str(team->uuid))  << "\",\n";
        file << "      \"name\": \""         << json_escape(team->name)               << "\",\n";
        file << "      \"description\": \"" << json_escape(team->description)         << "\"\n";
        file << "    }";
    }

    //je write les infos des channels dans le json
    file << "\n  ],\n";
    file << "  \"channels\": [\n";

    first = true;
    for (const auto &channel : _channels) {
        if (!channel)
            continue;
        if (!first)
            file << ",\n";
        first = false;
        file << "    {\n";
        file << "      \"team_uuid\": \""    << json_escape(uuid_to_str(channel->team_uuid))        << "\",\n";
        file << "      \"uuid\": \""          << json_escape(uuid_to_str(channel->uuid))             << "\",\n";
        file << "      \"name\": \""          << json_escape(channel->channel_name)                  << "\",\n";
        file << "      \"description\": \""  << json_escape(channel->channel_description)            << "\"\n";
        file << "    }";
    }

    //je write les infos des threads dans le json
    file << "\n  ],\n";
    file << "  \"threads\": [\n";

    first = true;
    for (const auto &thread : _threads) {
        if (!thread)
            continue;
        if (!first)
            file << ",\n";
        first = false;
        file << "    {\n";
        file << "      \"team_uuid\": \""    << json_escape(uuid_to_str(thread->team_uuid))    << "\",\n";
        file << "      \"channel_uuid\": \"" << json_escape(uuid_to_str(thread->channel_uuid)) << "\",\n";
        file << "      \"uuid\": \""          << json_escape(uuid_to_str(thread->uuid))         << "\",\n";
        file << "      \"title\": \""         << json_escape(thread->thread_title)               << "\",\n";
        file << "      \"message\": \""       << json_escape(thread->thread_message)             << "\",\n";
        file << "      \"timestamp\": "       << static_cast<long long>(thread->timestamp)       << "\n";
        file << "    }";
    }

    //je write les infos des replies dans le json
    file << "\n  ],\n";
    file << "  \"replies\": [\n";

    first = true;
    for (const auto &reply : _replies) {
        if (!reply)
            continue;
        if (!first)
            file << ",\n";
        first = false;
        file << "    {\n";
        file << "      \"thread_uuid\": \"" << json_escape(uuid_to_str(reply->thread_uuid)) << "\",\n";
        file << "      \"uuid\": \""         << json_escape(uuid_to_str(reply->uuid))         << "\",\n";
        file << "      \"message\": \""      << json_escape(reply->reply_message)             << "\",\n";
        file << "      \"timestamp\": "      << static_cast<long long>(reply->timestamp)      << "\n";
        file << "    }";
    }

    //je write les infos des private messages dans le json
    file << "\n  ],\n";
    file << "  \"private_messages\": [\n";

    first = true;
    for (const auto &pm : _private_messages) {
        if (!pm)
            continue;
        if (!first)
            file << ",\n";
        first = false;
        file << "    {\n";
        file << "      \"sender_uuid\": \""   << json_escape(uuid_to_str(pm->sender_uuid))   << "\",\n";
        file << "      \"receiver_uuid\": \"" << json_escape(uuid_to_str(pm->receiver_uuid)) << "\",\n";
        file << "      \"body\": \""           << json_escape(pm->body)                       << "\",\n";
        file << "      \"timestamp\": "        << static_cast<long long>(pm->timestamp)       << "\n";
        file << "    }";
    }
    file << "\n  ]\n";
    file << "}\n";
    file.close();
    std::cout << "[INFO] State saved to server_save.json" << std::endl;
}

static void skip_whitespace(const std::string &s, size_t &i)
{
    for (; i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r'); i++);
}

static std::string parse_string(const std::string &s, size_t &i)
{
    if (i >= s.size() || s[i] != '"')
        throw std::runtime_error("Expected '\"' at position " + std::to_string(i));
    i++;
    std::string result;
    while (i < s.size() && s[i] != '"') {
        if (s[i] == '\\') {
            i++;
            if (i >= s.size())
                break;
            switch (s[i]) {
                case '"':  result += '"'; 
                    break;
                case '\\': result += '\\'; 
                    break;
                case 'n':  result += '\n'; 
                    break;
                case 'r':  result += '\r'; 
                    break;
                case 't':  result += '\t'; 
                    break;
                default:   result += s[i]; 
                    break;
            }
        } else
            result += s[i];
        i++;
    }
    if (i < s.size())
        i++;
    return result;
}

static long long parse_number(const std::string &s, size_t &i)
{
    size_t start = i;
    if (i < s.size() && s[i] == '-')
        i++;
    for (; i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])); i++);
    return std::stoll(s.substr(start, i - start));
}

static bool parse_bool(const std::string &s, size_t &i)
{
    if (s.substr(i, 4) == "true") {
        i += 4;
        return true;
    }
    if (s.substr(i, 5) == "false") {
        i += 5;
        return false;
    }
    throw std::runtime_error("Expected bool at position " + std::to_string(i));
}

static void skip_comma(const std::string &s, size_t &i)
{
    skip_whitespace(s, i);
    if (i < s.size() && s[i] == ',')
        i++;
}

static std::string expect_kv_str(const std::string &s, size_t &i, const std::string &expected)
{
    skip_whitespace(s, i);
    std::string key = parse_string(s, i);

    if (key != expected)
        throw std::runtime_error("Expected key '" + expected + "', got '" + key + "'");
    skip_whitespace(s, i);
    if (i >= s.size() || s[i] != ':')
        throw std::runtime_error("Expected ':' after key '" + expected + "'");
    i++;
    skip_whitespace(s, i);
    return parse_string(s, i);
}

static long long expect_kv_num(const std::string &s, size_t &i, const std::string &expected)
{
    skip_whitespace(s, i);
    std::string key = parse_string(s, i);
    if (key != expected)
        throw std::runtime_error("Expected key '" + expected + "', got '" + key + "'");
    skip_whitespace(s, i);
    if (i >= s.size() || s[i] != ':')
        throw std::runtime_error("Expected ':' after key '" + expected + "'");
    i++;
    skip_whitespace(s, i);
    return parse_number(s, i);
}

static bool expect_kv_bool(const std::string &s, size_t &i, const std::string &expected)
{
    skip_whitespace(s, i);
    std::string key = parse_string(s, i);
    if (key != expected)
        throw std::runtime_error("Expected key '" + expected + "', got '" + key + "'");
    skip_whitespace(s, i);
    if (i >= s.size() || s[i] != ':')
        throw std::runtime_error("Expected ':' after key '" + expected + "'");
    i++;
    skip_whitespace(s, i);
    return parse_bool(s, i);
}


static void load_users(Server &server, const std::string &json, size_t &i)
{    
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{')
            throw std::runtime_error("Expected '{' in users array");
        i++;
        User *user = new User;
        memset(user, 0, sizeof(*user));
        std::string uuid_str = expect_kv_str(json, i, "uuid");
        skip_comma(json, i);
        std::string username  = expect_kv_str(json, i, "username");
        skip_comma(json, i);
        bool is_online = expect_kv_bool(json, i, "is_online");
        uuid_parse(uuid_str.c_str(), user->uuid);
        std::strncpy(user->username, username.c_str(), MAX_NAME_LENGTH - 1);
        user->username[MAX_NAME_LENGTH - 1] = '\0';
        user->is_online = is_online;
        server.get_users().push_back(user);
        server_event_user_loaded(uuid_str.c_str(), user->username);
        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}')
            i++;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']')
        i++;
}

static void load_teams(Server &server, const std::string &json, size_t &i)
{
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{') throw std::runtime_error("Expected '{'");
        i++;
        Team *team = new Team;
        team->name[0] = '\0';
        team->description[0] = '\0';
        memset(team->uuid, 0, sizeof(team->uuid));
        std::string uuid_str = expect_kv_str(json, i, "uuid");
        skip_comma(json, i);
        std::string name = expect_kv_str(json, i, "name");
        skip_comma(json, i);
        std::string description = expect_kv_str(json, i, "description");
        uuid_parse(uuid_str.c_str(), team->uuid);
        std::strncpy(team->name, name.c_str(), MAX_NAME_LENGTH - 1);
        std::strncpy(team->description, description.c_str(), MAX_DESCRIPTION_LENGTH - 1);
        team->name[MAX_NAME_LENGTH - 1] = '\0';
        team->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';
        server.get_teams().push_back(team);
        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}')
            i++;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']')
        i++;
}

static void load_channels(Server &server, const std::string &json, size_t &i)
{
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{')
            throw std::runtime_error("Expected '{'");
        i++;

        Channel *ch = new Channel;
        memset(ch->team_uuid, 0, sizeof(ch->team_uuid));
        memset(ch->uuid, 0, sizeof(ch->uuid));
        ch->channel_name[0] = '\0';
        ch->channel_description[0] = '\0';

        std::string team_uuid_str = expect_kv_str(json, i, "team_uuid");
        skip_comma(json, i);
        std::string uuid_str      = expect_kv_str(json, i, "uuid");
        skip_comma(json, i);
        std::string name          = expect_kv_str(json, i, "name");
        skip_comma(json, i);
        std::string description   = expect_kv_str(json, i, "description");

        uuid_parse(team_uuid_str.c_str(), ch->team_uuid);
        uuid_parse(uuid_str.c_str(),      ch->uuid);
        std::strncpy(ch->channel_name,        name.c_str(),        MAX_NAME_LENGTH - 1);
        std::strncpy(ch->channel_description, description.c_str(), MAX_DESCRIPTION_LENGTH - 1);
        ch->channel_name[MAX_NAME_LENGTH - 1]               = '\0';
        ch->channel_description[MAX_DESCRIPTION_LENGTH - 1] = '\0';

        server.get_channels().push_back(ch);

        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}')
            i++;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']')
        i++;
}

static void load_threads(Server &server, const std::string &json, size_t &i)
{
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{')
            throw std::runtime_error("Expected '{'");
        i++;
        Thread *th = new Thread;
        memset(th->team_uuid,    0, sizeof(th->team_uuid));
        memset(th->channel_uuid, 0, sizeof(th->channel_uuid));
        memset(th->uuid,         0, sizeof(th->uuid));
        th->thread_title[0]   = '\0';
        th->thread_message[0] = '\0';
        th->timestamp = 0;

        std::string team_uuid_str    = expect_kv_str(json, i, "team_uuid");
        skip_comma(json, i);
        std::string channel_uuid_str = expect_kv_str(json, i, "channel_uuid");
        skip_comma(json, i);
        std::string uuid_str         = expect_kv_str(json, i, "uuid");
        skip_comma(json, i);
        std::string title            = expect_kv_str(json, i, "title");
        skip_comma(json, i);
        std::string message          = expect_kv_str(json, i, "message");
        skip_comma(json, i);
        long long   timestamp        = expect_kv_num(json, i, "timestamp");

        uuid_parse(team_uuid_str.c_str(),    th->team_uuid);
        uuid_parse(channel_uuid_str.c_str(), th->channel_uuid);
        uuid_parse(uuid_str.c_str(),         th->uuid);
        std::strncpy(th->thread_title,   title.c_str(),   MAX_NAME_LENGTH - 1);
        std::strncpy(th->thread_message, message.c_str(), MAX_DESCRIPTION_LENGTH - 1);
        th->thread_title[MAX_NAME_LENGTH - 1]         = '\0';
        th->thread_message[MAX_DESCRIPTION_LENGTH - 1] = '\0';
        th->timestamp = static_cast<time_t>(timestamp);

        server.get_threads().push_back(th);

        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}')
            i++;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']')
        i++;
}

static void load_replies(Server &server, const std::string &json, size_t &i)
{
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{')
            throw std::runtime_error("Expected '{'");
        i++;

        Reply *reply = new Reply;
        memset(reply->thread_uuid, 0, sizeof(reply->thread_uuid));
        memset(reply->uuid,        0, sizeof(reply->uuid));
        reply->reply_message[0] = '\0';
        reply->timestamp = 0;

        std::string thread_uuid_str = expect_kv_str(json, i, "thread_uuid");
        skip_comma(json, i);
        std::string uuid_str        = expect_kv_str(json, i, "uuid");
        skip_comma(json, i);
        std::string message         = expect_kv_str(json, i, "message");
        skip_comma(json, i);
        long long   timestamp       = expect_kv_num(json, i, "timestamp");

        uuid_parse(thread_uuid_str.c_str(), reply->thread_uuid);
        uuid_parse(uuid_str.c_str(),        reply->uuid);
        std::strncpy(reply->reply_message, message.c_str(), MAX_DESCRIPTION_LENGTH - 1);
        reply->reply_message[MAX_DESCRIPTION_LENGTH - 1] = '\0';
        reply->timestamp = static_cast<time_t>(timestamp);

        server.get_replies().push_back(reply);

        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}')
            i++;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']')
        i++;
}

static void load_private_messages(Server &server, const std::string &json, size_t &i)
{
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{')
            throw std::runtime_error("Expected '{'");
        i++;

        PrivateMessage *pm = new PrivateMessage;
        memset(pm, 0, sizeof(*pm));

        std::string sender_uuid_str   = expect_kv_str(json, i, "sender_uuid");
        skip_comma(json, i);
        std::string receiver_uuid_str = expect_kv_str(json, i, "receiver_uuid");
        skip_comma(json, i);
        std::string body              = expect_kv_str(json, i, "body");
        skip_comma(json, i);
        long long   timestamp         = expect_kv_num(json, i, "timestamp");

        uuid_parse(sender_uuid_str.c_str(),   pm->sender_uuid);
        uuid_parse(receiver_uuid_str.c_str(), pm->receiver_uuid);
        std::strncpy(pm->body, body.c_str(), MAX_BODY_LENGTH - 1);
        pm->body[MAX_BODY_LENGTH - 1] = '\0';
        pm->timestamp = static_cast<time_t>(timestamp);

        server.get_private_messages().push_back(pm);

        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}') ++i;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']') ++i;
}

void Server::load_state()
{
    std::ifstream file("server_save.json");
    if (!file.is_open()) {
        std::cout << "[INFO] No save file found, starting fresh." << std::endl;
        return;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();
    file.close();

    try {
        size_t i = 0;
        skip_whitespace(json, i);
        if (i >= json.size() || json[i] != '{')
            throw std::runtime_error("Expected '{' at start");
        i++;

        while (i < json.size()) {
            skip_whitespace(json, i);
            if (i >= json.size() || json[i] == '}')
                break;

            std::string key = parse_string(json, i);
            skip_whitespace(json, i);
            if (i >= json.size() || json[i] != ':')
                throw std::runtime_error("Expected ':' after key '" + key + "'");
            ++i;
            skip_whitespace(json, i);

            if      (key == "users")
                load_users(          *this, json, i);
            else if (key == "teams")
                load_teams(          *this, json, i);
            else if (key == "channels")
                load_channels(       *this, json, i);
            else if (key == "threads")
                load_threads(        *this, json, i);
            else if (key == "replies")
                load_replies(        *this, json, i);
            else if (key == "private_messages")
                load_private_messages(*this, json, i);
            else {
                int depth = 0;
                while (i < json.size()) {
                    char c = json[i++];
                    if (c == '[' || c == '{')
                        depth++;
                    else if (c == ']' || c == '}') {
                        if (depth == 0) {
                            --i;
                            break;
                        }
                        depth--;
                    }
                    else if (c == '"') {
                        --i;
                        parse_string(json, i);
                    }
                }
            }

            skip_comma(json, i);
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] Failed to load save file: " << e.what() << std::endl;
    }

    std::cout << "[INFO] Loaded " << _users.size()            << " user(s), "
              << _teams.size()            << " team(s), "
              << _channels.size()         << " channel(s), "
              << _threads.size()          << " thread(s), "
              << _replies.size()          << " repl(ies), "
              << _private_messages.size() << " private message(s)."
              << std::endl;
}
