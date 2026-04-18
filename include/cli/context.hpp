/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** context
*/

#ifndef CONTEXT_HPP_
#define CONTEXT_HPP_

#include <string>

namespace Cli {
    /**
     * @brief Maintains client-side context for user and navigation state
     */
    class Context {
    private:
        std::string _user_uuid;
        std::string _username;
        std::string _team_uuid;
        std::string _channel_uuid;
        std::string _thread_uuid;
        bool _should_quit = false;

    public:
        Context() = default;
        ~Context() = default;

        // User information setters/getters
        void set_user(const std::string& uuid, const std::string& username) {
            _user_uuid = uuid;
            _username = username;
        }

        const std::string& get_user_uuid() const { return _user_uuid; }
        const std::string& get_username() const { return _username; }
        bool is_logged_in() const { return !_user_uuid.empty(); }

        // Navigation context
        void set_team(const std::string& uuid) { _team_uuid = uuid; }
        void set_channel(const std::string& uuid) { _channel_uuid = uuid; }
        void set_thread(const std::string& uuid) { _thread_uuid = uuid; }

        const std::string& get_team_uuid() const { return _team_uuid; }
        const std::string& get_channel_uuid() const { return _channel_uuid; }
        const std::string& get_thread_uuid() const { return _thread_uuid; }

        // Reset context for logout
        void reset() {
            _user_uuid.clear();
            _username.clear();
            _team_uuid.clear();
            _channel_uuid.clear();
            _thread_uuid.clear();
        }

        // Exit control
        void set_should_quit(bool quit) { _should_quit = quit; }
        bool should_quit() const { return _should_quit; }

        // Clear navigation context
        void reset_navigation() {
            _team_uuid.clear();
            _channel_uuid.clear();
            _thread_uuid.clear();
        }
    };
}

#endif /* !CONTEXT_HPP_ */
