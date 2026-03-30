/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** client
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "../../libs/myteams/logging_client.h"
#include <string>

namespace IClient {
    class IClient {
        public:
            virtual ~IClient() = default;
            virtual bool GetClientStatus(void) = 0;
            virtual void SetClientStatus(bool status) = 0;
            virtual bool is_auth() const = 0;
            virtual void launch_client(std::string ip, std::string port) = 0;
    };
}

namespace Cli {
    class Client final : public IClient::IClient {
        private:
            bool _client_status = false;

        public:
            ~Client() override = default;

            bool GetClientStatus(void) override { return _client_status; }
            void SetClientStatus(bool status) override { _client_status = status; }
            bool is_auth() const override { return _client_status; }
            void launch_client(std::string ip, std::string port) override;


            // ECAPSULED FUNCTIONS

            int EventLoggedIn(char const *user_uuid, const char *user_name) const
            { return client_event_logged_in(user_uuid, user_name); }

            int EventLoggedOut(char const *user_uuid, const char *user_name) const
            { return client_event_logged_out(user_uuid, user_name); }

            int EventPrivateMessageReceived(char const *user_uuid, char const *message_body) const
            { return client_event_private_message_received(user_uuid, message_body); }

            int EventThreadReplyReceived(char const *team_uuid, char const *thread_uuid,
                char const *user_uuid, char const *reply_body) const
            { return client_event_thread_reply_received(team_uuid, thread_uuid, user_uuid, reply_body); }

            int EventTeamCreated(char const *team_uuid, char const *team_name, char const *team_description) const
            { return client_event_team_created(team_uuid, team_name, team_description); }

            int EventChannelCreated(char const *channel_uuid, char const *channel_name,
                char const *channel_description) const
            { return client_event_channel_created(channel_uuid, channel_name, channel_description); }

            int EventThreadCreated(char const *thread_uuid, char const *user_uuid, time_t thread_timestamp,
                char const *thread_title, char const *thread_body) const
            { return client_event_thread_created(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body); }

            int PrintUsers(char const *user_uuid, char const *user_name, int user_status) const
            { return client_print_users(user_uuid, user_name, user_status); }

            int PrintTeams(char const *team_uuid, char const *team_name, char const *team_description) const
            { return client_print_teams(team_uuid, team_name, team_description); }

            int TeamPrintChannels(char const *channel_uuid, char const *channel_name,
                char const *channel_description) const
            { return client_team_print_channels(channel_uuid, channel_name, channel_description); }

            int ChannelPrintThreads(char const *thread_uuid, char const *user_uuid, time_t thread_timestamp,
                char const *thread_title, char const *thread_body) const
            { return client_channel_print_threads(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body); }

            int ThreadPrintReplies(char const *thread_uuid, char const *user_uuid, time_t reply_timestamp,
                char const *reply_body) const
            { return client_thread_print_replies(thread_uuid, user_uuid, reply_timestamp, reply_body); }

            int PrivateMessagePrintMessages(char const *sender_uuid, time_t message_timestamp,
                char const *message_body) const
            { return client_private_message_print_messages(sender_uuid, message_timestamp, message_body); }

            int ErrorUnknownTeam(char const *team_uuid) const { return client_error_unknown_team(team_uuid); }
            int ErrorUnknownChannel(char const *channel_uuid) const { return client_error_unknown_channel(channel_uuid); }
            int ErrorUnknownThread(char const *thread_uuid) const { return client_error_unknown_thread(thread_uuid); }
            int ErrorUnknownUser(char const *user_uuid) const { return client_error_unknown_user(user_uuid); }
            int ErrorUnauthorized(void) const { return client_error_unauthorized(); }
            int ErrorAlreadyExist(void) const { return client_error_already_exist(); }

            int PrintUser(char const *user_uuid, char const *user_name, int user_status) const
            { return client_print_user(user_uuid, user_name, user_status); }

            int PrintTeam(char const *team_uuid, char const *team_name, char const *team_description) const
            { return client_print_team(team_uuid, team_name, team_description); }

            int PrintChannel(char const *channel_uuid, char const *channel_name, char const *channel_description) const
            { return client_print_channel(channel_uuid, channel_name, channel_description); }

            int PrintThread(char const *thread_uuid, char const *user_uuid, time_t thread_timestamp,
                char const *thread_title, char const *thread_body) const
            { return client_print_thread(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body); }

            int PrintTeamCreated(char const *team_uuid, char const *team_name, char const *team_description) const
            { return client_print_team_created(team_uuid, team_name, team_description); }

            int PrintChannelCreated(char const *channel_uuid, char const *channel_name,
                char const *channel_description) const
            { return client_print_channel_created(channel_uuid, channel_name, channel_description); }

            int PrintThreadCreated(char const *thread_uuid, char const *user_uuid, time_t thread_timestamp,
                char const *thread_title, char const *thread_body) const
            { return client_print_thread_created(thread_uuid, user_uuid, thread_timestamp, thread_title, thread_body); }

            int PrintReplyCreated(char const *thread_uuid, char const *user_uuid, time_t reply_timestamp,
                char const *reply_body) const
            { return client_print_reply_created(thread_uuid, user_uuid, reply_timestamp, reply_body); }

            int PrintSubscribed(char const *user_uuid, char const *team_uuid) const
            { return client_print_subscribed(user_uuid, team_uuid); }

            int PrintUnsubscribed(char const *user_uuid, char const *team_uuid) const
            { return client_print_unsubscribed(user_uuid, team_uuid); }
    };
}

#endif /* !CLIENT_HPP_ */
