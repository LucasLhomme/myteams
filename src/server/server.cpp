/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** server
*/

extern "C" {
    #include "../../libs/myteams/logging_server.h"
}

namespace Server {
    class Logger final {
        public:
            ~Logger() = default;

            int EventTeamCreated(
                char const *team_uuid,
                char const *team_name,
                char const *user_uuid) const
            { return server_event_team_created(team_uuid, team_name, user_uuid); }

            int EventChannelCreated(
                char const *team_uuid,
                char const *channel_uuid,
                char const *channel_name) const
            { return server_event_channel_created(team_uuid, channel_uuid, channel_name); }

            int EventThreadCreated(
                char const *channel_uuid,
                char const *thread_uuid,
                char const *user_uuid,
                char const *thread_title,
                char const *thread_body) const
            { return server_event_thread_created(channel_uuid, thread_uuid, user_uuid, thread_title, thread_body); }

            int EventReplyCreated(
                char const *thread_uuid,
                char const *user_uuid,
                char const *reply_body) const
            { return server_event_reply_created(thread_uuid, user_uuid, reply_body); }

            int EventUserSubscribed(char const *team_uuid, char const *user_uuid) const
            { return server_event_user_subscribed(team_uuid, user_uuid); }

            int EventUserUnsubscribed(char const *team_uuid, char const *user_uuid) const
            { return server_event_user_unsubscribed(team_uuid, user_uuid); }

            int EventUserCreated(char const *user_uuid, char const *user_name) const
            { return server_event_user_created(user_uuid, user_name); }

            int EventUserLoaded(char const *user_uuid, char const *user_name) const
            { return server_event_user_loaded(user_uuid, user_name); }

            int EventUserLoggedIn(char const *user_uuid) const
            { return server_event_user_logged_in(user_uuid); }

            int EventUserLoggedOut(char const *user_uuid) const
            { return server_event_user_logged_out(user_uuid); }

            int EventPrivateMessageSended(
                char const *sender_uuid,
                char const *receiver_uuid,
                char const *message_body) const
            { return server_event_private_message_sended(sender_uuid, receiver_uuid, message_body); }
    };
}

