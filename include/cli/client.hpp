/*
** EPITECH PROJECT, 2026
** MY-TEAMS-1
** File description:
** client
*/

#ifndef CLIENT_HPP_
#define CLIENT_HPP_
extern "C" {
#include "../../libs/myteams/logging_client.h"
}
#include "./context.hpp"
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
            Context _context;

        public:
            ~Client() override = default;

            bool GetClientStatus(void) override { return _client_status; }
            void SetClientStatus(bool status) override { _client_status = status; }
            bool is_auth() const override { return _client_status; }
            void launch_client(std::string ip, std::string port) override;

            Context& get_context() { return _context; }
            const Context& get_context() const { return _context; }
    };
}

#endif /* !CLIENT_HPP_ */
