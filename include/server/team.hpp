/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** team
*/

#ifndef TEAM_HPP_
#define TEAM_HPP_

#include <uuid/uuid.h>
#include <ctime>
#include "constants.hpp"

class ATeam {
  protected:
    uuid_t uuid;
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
  public:
    virtual ~ATeam() = default;
};

struct Team : public ATeam {
  uuid_t uuid;
  char name[MAX_NAME_LENGTH];
  char description[MAX_DESCRIPTION_LENGTH];
};

struct Channel : public ATeam {
  uuid_t team_uuid;
  uuid_t uuid;
  char channel_name[MAX_NAME_LENGTH];
  char channel_description[MAX_DESCRIPTION_LENGTH];
};

struct Thread : public ATeam {
  uuid_t team_uuid;
  uuid_t channel_uuid;
  uuid_t uuid;
  char thread_title[MAX_NAME_LENGTH];
  char thread_message[MAX_DESCRIPTION_LENGTH];
  time_t timestamp;
};

struct Reply : public ATeam {
  uuid_t thread_uuid;
  uuid_t uuid;
  char reply_message[MAX_DESCRIPTION_LENGTH];
  time_t timestamp;
};

#endif /* !TEAM_HPP_ */