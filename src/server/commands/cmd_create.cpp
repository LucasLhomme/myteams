/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** cmd_create
*/

#include "server/commands/cmd_create.hpp"
#include "server/server.hpp"
#include "server/team.hpp"
#include <cstring>
#include <memory>

extern "C" {
    #include "../../../libs/myteams/logging_server.h"
}

enum class CreateShape {
    TWO_FIELDS,
    REPLY_ONLY,
    INVALID
};
class ICreateCheck {
public:
    virtual bool check(const std::string &input) const = 0;
    virtual ~ICreateCheck() = default;
};

class NameCheck : public ICreateCheck {
public:
    bool check(const std::string &name) const override {
        return name.length() <= MAX_NAME_LENGTH;
    }
};

class DescriptionCheck : public ICreateCheck {
public:
    bool check(const std::string &description) const override {
        return description.length() <= MAX_DESCRIPTION_LENGTH;
    }
};

class BodyCheck : public ICreateCheck {
public:
    bool check(const std::string &body) const override {
        return body.length() <= MAX_BODY_LENGTH;
    }
};

static CreateShape get_create_shape(const std::vector<std::string> &args)
{
    if (args.size() == 3) {
        return CreateShape::TWO_FIELDS;
    }
    if (args.size() == 2) {
        return CreateShape::REPLY_ONLY;
    }
    return CreateShape::INVALID;
}

static std::vector<std::unique_ptr<ICreateCheck>> build_checks(CreateShape shape)
{
    std::vector<std::unique_ptr<ICreateCheck>> checks;

    if (shape == CreateShape::TWO_FIELDS) {
        checks.push_back(std::make_unique<NameCheck>());
        checks.push_back(std::make_unique<DescriptionCheck>());
    } else if (shape == CreateShape::REPLY_ONLY) {
        checks.push_back(std::make_unique<BodyCheck>());
    }
    return checks;
}

static bool check_args(const std::vector<std::string> &args) {
    const CreateShape shape = get_create_shape(args);

    if (shape == CreateShape::INVALID) {
        return false;
    }
    std::vector<std::unique_ptr<ICreateCheck>> checks = build_checks(shape);
    if (args.size() != checks.size() + 1) {
        return false;
    }
    for (size_t i = 0; i < checks.size(); ++i) {
        if (!checks[i]->check(args[i + 1])) {
            return false;
        }
    }
    return true;
}

static void create_team(Server &server, int client_socket, const std::vector<std::string> &args) {
    Team *team = new Team;
    uuid_generate(team->uuid);
    std::strncpy(team->name, args[1].c_str(), MAX_NAME_LENGTH);
    std::strncpy(team->description, args[2].c_str(), MAX_DESCRIPTION_LENGTH);
    server.get_teams().push_back(team);
    char u_str[MAX_UUID_LENGTH];
    uuid_unparse(team->uuid, u_str);
    const auto &socket_to_uuid = server.get_socket_to_uuid();
    const auto it = socket_to_uuid.find(client_socket);
    const char *user_uuid = (it != socket_to_uuid.end()) ? it->second.c_str() : "";
    server_event_team_created(u_str, team->name, user_uuid);
    server.send_response(client_socket, "Team created with UUID: " + std::string(u_str));
}

static void create_channel(Server &server, const std::vector<std::string> &args) {
    return; // TODO
}

static void create_thread(Server &server, const std::vector<std::string> &args) {
    return; // TODO
}

static void create_reply(Server &server, int client_socket, const std::vector<std::string> &args) {
    (void)server;
    server.send_response(client_socket, "Reply received: " + args[1]);
}

void CmdCreate::execute(Server &server, int client_socket, const std::vector<std::string> &args)
{

    if (!check_args(args)) {
        server.send_response(client_socket, "Invalid arguments for create command");
        return;
    }

    if (args.size() == 3) {
        create_team(server, client_socket, args);
    } else if (args.size() == 2) {
        create_reply(server, client_socket, args);
    }
    server.save_state();
}