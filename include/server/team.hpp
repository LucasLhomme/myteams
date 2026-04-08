/*
** EPITECH PROJECT, 2026
** my_teams
** File description:
** team
*/

#ifndef TEAM_HPP_
#define TEAM_HPP_

#include <uuid/uuid.h>
#include "constants.hpp"

struct Team {
  uuid_t uuid;
  char name[MAX_NAME_LENGTH];
  char description[MAX_DESCRIPTION_LENGTH];
};

#endif /* !TEAM_HPP_ */