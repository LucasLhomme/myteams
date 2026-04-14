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
    try {
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
    } catch (const std::exception &e) {
        std::cout << "[ERROR] json_escape: " << e.what() << std::endl;
        return s;
    }
}

//fonction qui convertit un uuid en string
static std::string uuid_to_str(const uuid_t u)
{
    try {
        char buf[MAX_UUID_LENGTH];

        uuid_unparse(u, buf);
        return std::string(buf);
    } catch (const std::exception &e) {
        std::cout << "[ERROR] uuid_to_str: " << e.what() << std::endl;
        return "";
    }
}


//c la fonction qui va ecrire dans le fichier json
template <typename T, typename Func>

static void save_array(std::ofstream &f, const std::string &name, const std::vector<T*> &vec, Func write_op, bool last = false)
{
    f << "  \"" << name << "\": [\n";
    bool first = true;
    for (const auto &item : vec) {
        if (!item)
            continue;
        if (!first)
            f << ",\n";
        first = false;
        f << "    {\n";
        write_op(f, item);
        f << "\n    }";
    }
    f << "\n  ]" << (last ? "\n" : ",\n");
}

void Server::save_state()
{
    try {
        std::ofstream file("server_save.json");
        if (!file.is_open()) {
            std::cout << "[ERROR] Cannot open server_save.json for writing" << std::endl;
            return;
        }

        file << "{\n";

        save_array(file, "users", _users, [](std::ofstream &f, const User *u) {
            f << "      \"uuid\": \"" << json_escape(uuid_to_str(u->uuid)) << "\",\n"
              << "      \"username\": \"" << json_escape(u->username) << "\",\n"
              << "      \"is_online\": " << (u->is_online ? "true" : "false");
        });

        save_array(file, "teams", _teams, [](std::ofstream &f, const Team *t) {
            f << "      \"uuid\": \"" << json_escape(uuid_to_str(t->uuid)) << "\",\n"
              << "      \"name\": \"" << json_escape(t->name) << "\",\n"
              << "      \"description\": \"" << json_escape(t->description) << "\"";
        });

        save_array(file, "channels", _channels, [](std::ofstream &f, const Channel *c) {
            f << "      \"team_uuid\": \"" << json_escape(uuid_to_str(c->team_uuid)) << "\",\n"
              << "      \"uuid\": \"" << json_escape(uuid_to_str(c->uuid)) << "\",\n"
              << "      \"name\": \"" << json_escape(c->channel_name) << "\",\n"
              << "      \"description\": \"" << json_escape(c->channel_description) << "\"";
        });

        save_array(file, "threads", _threads, [](std::ofstream &f, const Thread *th) {
            f << "      \"team_uuid\": \"" << json_escape(uuid_to_str(th->team_uuid)) << "\",\n"
              << "      \"channel_uuid\": \"" << json_escape(uuid_to_str(th->channel_uuid)) << "\",\n"
              << "      \"uuid\": \"" << json_escape(uuid_to_str(th->uuid)) << "\",\n"
              << "      \"title\": \"" << json_escape(th->thread_title) << "\",\n"
              << "      \"message\": \"" << json_escape(th->thread_message) << "\",\n"
              << "      \"timestamp\": " << static_cast<long long>(th->timestamp);
        });

        save_array(file, "replies", _replies, [](std::ofstream &f, const Reply *r) {
            f << "      \"thread_uuid\": \"" << json_escape(uuid_to_str(r->thread_uuid)) << "\",\n"
              << "      \"uuid\": \"" << json_escape(uuid_to_str(r->uuid)) << "\",\n"
              << "      \"message\": \"" << json_escape(r->reply_message) << "\",\n"
              << "      \"timestamp\": " << static_cast<long long>(r->timestamp);
        });

        save_array(file, "private_messages", _private_messages, [](std::ofstream &f, const PrivateMessage *pm) {
            f << "      \"sender_uuid\": \"" << json_escape(uuid_to_str(pm->sender_uuid)) << "\",\n"
              << "      \"receiver_uuid\": \"" << json_escape(uuid_to_str(pm->receiver_uuid)) << "\",\n"
              << "      \"body\": \"" << json_escape(pm->body) << "\",\n"
              << "      \"timestamp\": " << static_cast<long long>(pm->timestamp);
        }, true);

        file << "}\n";
        file.close();
        std::cout << "[INFO] State saved to server_save.json" << std::endl;
    } catch (const std::exception &e) {
        std::cout << "[ERROR] save_state exception: " << e.what() << std::endl;
    }
}

static void skip_whitespace(const std::string &s, size_t &i)
{
    for (; i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r'); i++);
}

static std::string parse_string(const std::string &s, size_t &i)
{
    try {
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
    } catch (const std::exception &e) {
        std::cout << "[ERROR] parse_string: " << e.what() << std::endl;
        throw;
    }
}

static long long parse_number(const std::string &s, size_t &i)
{
    try {
        size_t start = i;
        if (i < s.size() && s[i] == '-')
            i++;
        for (; i < s.size() && std::isdigit(static_cast<unsigned char>(s[i])); i++);
        return std::stoll(s.substr(start, i - start));
    } catch (const std::exception &e) {
        std::cout << "[ERROR] parse_number: " << e.what() << std::endl;
        throw;
    }
}

static bool parse_bool(const std::string &s, size_t &i)
{
    try {
        if (s.substr(i, 4) == "true") {
            i += 4;
            return true;
        }
        if (s.substr(i, 5) == "false") {
            i += 5;
            return false;
        }
        throw std::runtime_error("Expected bool at position " + std::to_string(i));
    } catch (const std::exception &e) {
        std::cout << "[ERROR] parse_bool: " << e.what() << std::endl;
        throw;
    }
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


template <typename Func>
static void parse_json_array(const std::string &json, size_t &i, Func parse_element)
{
    i++;
    skip_whitespace(json, i);
    while (i < json.size() && json[i] != ']') {
        if (json[i] != '{')
            throw std::runtime_error("Expected '{'");
        i++;
        parse_element();
        skip_whitespace(json, i);
        if (i < json.size() && json[i] == '}')
            i++;
        skip_comma(json, i);
        skip_whitespace(json, i);
    }
    if (i < json.size() && json[i] == ']')
        i++;
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
            i++;
            skip_whitespace(json, i);

            if (key == "users") {
                parse_json_array(json, i, [&]() {
                    User *user = new User;
                    std::string uu = expect_kv_str(json, i, "uuid"); skip_comma(json, i);
                    std::string un = expect_kv_str(json, i, "username"); skip_comma(json, i);
                    user->is_online = expect_kv_bool(json, i, "is_online");
                    uuid_parse(uu.c_str(), user->uuid);
                    std::strncpy(user->username, un.c_str(), MAX_NAME_LENGTH - 1);
                    user->username[MAX_NAME_LENGTH - 1] = '\0';
                    user->subscribed_teams.clear();
                    _users.push_back(user);
                    server_event_user_loaded(uu.c_str(), user->username);
                });
            } else if (key == "teams") {
                parse_json_array(json, i, [&]() {
                    Team *t = new Team;
                    std::string uu = expect_kv_str(json, i, "uuid"); skip_comma(json, i);
                    std::string n  = expect_kv_str(json, i, "name"); skip_comma(json, i);
                    std::string d  = expect_kv_str(json, i, "description");
                    uuid_parse(uu.c_str(), t->uuid);
                    std::strncpy(t->name, n.c_str(), MAX_NAME_LENGTH - 1);
                    std::strncpy(t->description, d.c_str(), MAX_DESCRIPTION_LENGTH - 1);
                    t->name[MAX_NAME_LENGTH - 1] = '\0';
                    t->description[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                    _teams.push_back(t);
                });
            } else if (key == "channels") {
                parse_json_array(json, i, [&]() {
                    Channel *c = new Channel;
                    std::string tu = expect_kv_str(json, i, "team_uuid"); skip_comma(json, i);
                    std::string uu = expect_kv_str(json, i, "uuid"); skip_comma(json, i);
                    std::string n  = expect_kv_str(json, i, "name"); skip_comma(json, i);
                    std::string d  = expect_kv_str(json, i, "description");
                    uuid_parse(tu.c_str(), c->team_uuid);
                    uuid_parse(uu.c_str(), c->uuid);
                    std::strncpy(c->channel_name, n.c_str(), MAX_NAME_LENGTH - 1);
                    std::strncpy(c->channel_description, d.c_str(), MAX_DESCRIPTION_LENGTH - 1);
                    c->channel_name[MAX_NAME_LENGTH - 1] = '\0';
                    c->channel_description[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                    _channels.push_back(c);
                });
            } else if (key == "threads") {
                parse_json_array(json, i, [&]() {
                    Thread *th = new Thread;
                    std::string tu = expect_kv_str(json, i, "team_uuid"); skip_comma(json, i);
                    std::string cu = expect_kv_str(json, i, "channel_uuid"); skip_comma(json, i);
                    std::string uu = expect_kv_str(json, i, "uuid"); skip_comma(json, i);
                    std::string t  = expect_kv_str(json, i, "title"); skip_comma(json, i);
                    std::string m  = expect_kv_str(json, i, "message"); skip_comma(json, i);
                    th->timestamp = expect_kv_num(json, i, "timestamp");
                    uuid_parse(tu.c_str(), th->team_uuid);
                    uuid_parse(cu.c_str(), th->channel_uuid);
                    uuid_parse(uu.c_str(), th->uuid);
                    std::strncpy(th->thread_title, t.c_str(), MAX_NAME_LENGTH - 1);
                    std::strncpy(th->thread_message, m.c_str(), MAX_DESCRIPTION_LENGTH - 1);
                    th->thread_title[MAX_NAME_LENGTH - 1] = '\0';
                    th->thread_message[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                    _threads.push_back(th);
                });
            } else if (key == "replies") {
                parse_json_array(json, i, [&]() {
                    Reply *r = new Reply;
                    std::string tu = expect_kv_str(json, i, "thread_uuid"); skip_comma(json, i);
                    std::string uu = expect_kv_str(json, i, "uuid"); skip_comma(json, i);
                    std::string m  = expect_kv_str(json, i, "message"); skip_comma(json, i);
                    r->timestamp = expect_kv_num(json, i, "timestamp");
                    uuid_parse(tu.c_str(), r->thread_uuid);
                    uuid_parse(uu.c_str(), r->uuid);
                    std::strncpy(r->reply_message, m.c_str(), MAX_DESCRIPTION_LENGTH - 1);
                    r->reply_message[MAX_DESCRIPTION_LENGTH - 1] = '\0';
                    _replies.push_back(r);
                });
            } else if (key == "private_messages") {
                parse_json_array(json, i, [&]() {
                    PrivateMessage *pm = new PrivateMessage;
                    memset(pm, 0, sizeof(*pm));
                    std::string su = expect_kv_str(json, i, "sender_uuid"); skip_comma(json, i);
                    std::string ru = expect_kv_str(json, i, "receiver_uuid"); skip_comma(json, i);
                    std::string b  = expect_kv_str(json, i, "body"); skip_comma(json, i);
                    pm->timestamp = expect_kv_num(json, i, "timestamp");
                    uuid_parse(su.c_str(), pm->sender_uuid);
                    uuid_parse(ru.c_str(), pm->receiver_uuid);
                    std::strncpy(pm->body, b.c_str(), MAX_BODY_LENGTH - 1);
                    pm->body[MAX_BODY_LENGTH - 1] = '\0';
                    _private_messages.push_back(pm);
                });
            } else {
                int depth = 0;
                while (i < json.size()) {
                    char c = json[i++];
                    if (c == '[' || c == '{')
                        depth++;
                    else if (c == ']' || c == '}') {
                        if (depth == 0) {
                            i--;
                            break;
                        }
                        depth--;
                    } else if (c == '"') { 
                        i--;
                        parse_string(json, i);
                    }
                }
            }
            skip_comma(json, i);
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] Failed to load save file: " << e.what() << std::endl;
    }

    std::cout << "[INFO] Loaded " << _users.size() << " user(s), "
              << _teams.size() << " team(s), "
              << _channels.size() << " channel(s), "
              << _threads.size() << " thread(s), "
              << _replies.size() << " repl(ies), "
              << _private_messages.size() << " private message(s)."
              << std::endl;
}
