/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** Command Factory
*/

#include "server/commands/CommandFactory.hpp"
#include "server/commands/cmd_send.hpp"
#include "server/commands/cmd_login.hpp"
#include "server/commands/cmd_logout.hpp"
#include "server/commands/cmd_users.hpp"
#include "server/commands/cmd_user.hpp"
#include "server/commands/cmd_messages.hpp"
#include "server/commands/cmd_subscribe.hpp"
#include "server/commands/cmd_subscribed.hpp"
#include "server/commands/cmd_unsubscribe.hpp"
#include "server/commands/cmd_help.hpp"
#include "server/commands/cmd_create.hpp"
#include "server/commands/cmd_list.hpp"
#include "server/commands/cmd_use.hpp"
#include "server/commands/cmd_info.hpp"
#include <iostream>

CommandFactory::CommandFactory() {
    try {
        _commands["/send"] = std::make_shared<CmdSend>();
        _commands["/login"] = std::make_shared<CmdLogin>();
        _commands["/logout"] = std::make_shared<CmdLogout>();
        _commands["/users"] = std::make_shared<CmdUsers>();
        _commands["/user"] = std::make_shared<CmdUser>();
        _commands["/messages"] = std::make_shared<CmdMessages>();
        _commands["/subscribe"] = std::make_shared<CmdSubscribe>();
        _commands["/subscribed"] = std::make_shared<CmdSubscribed>();
        _commands["/unsubscribe"] = std::make_shared<CmdUnsubscribe>();
        _commands["/help"] = std::make_shared<CmdHelp>();
        _commands["exit"] = std::make_shared<CmdLogout>();
        _commands["/create"] = std::make_shared<CmdCreate>();
        _commands["/list"] = std::make_shared<CmdList>();
        _commands["/use"] = std::make_shared<CmdUse>();
        _commands["/info"] = std::make_shared<CmdInfo>();
    } catch (const std::exception &e) {
        std::cout << "[ERROR] CommandFactory constructor: " << e.what() << std::endl;
    }
}

std::shared_ptr<ICommand> CommandFactory::get_command(const std::string &name) {
    try {
        auto it = _commands.find(name);
        if (it != _commands.end()) {
            return it->second;
        }
    } catch (const std::exception &e) {
        std::cout << "[ERROR] get_command: " << e.what() << std::endl;
    }
    return nullptr;
}
