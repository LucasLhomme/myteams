##
## EPITECH PROJECT, 2026
## MY-TEAMS-1
## File description:
## Makefile
##

SRC_SERVER  =   $(wildcard src/server/*.cpp) $(wildcard src/server/commands/*.cpp)
SRC_CLI     =   $(wildcard src/cli/*.cpp) $(wildcard src/cli/commands/*.cpp)

SRC_TESTS   =   $(wildcard tests/*.cpp)

OBJ_SERVER  =   $(SRC_SERVER:.cpp=.o)
OBJ_CLI     =   $(SRC_CLI:.cpp=.o)

NAME_SERVER =   myteams_server
NAME_CLI    =   myteams_cli
TEST_NAME   =   unit_tests

CXXFLAGS    =   -Wall -Wextra -Iinclude -g
CPPFLAGS    =   -Iinclude

LIB_MYTEAMS_DIR = libs/myteams

LDFLAGS    +=   -L$(LIB_MYTEAMS_DIR)
LDLIBS     +=   -lmyteams -luuid

CXX         =   g++
RM          =   rm -f

all:    $(NAME_SERVER) $(NAME_CLI)

$(NAME_SERVER):    $(OBJ_SERVER)
	$(CXX) -o $(NAME_SERVER) $(OBJ_SERVER) $(LDFLAGS) $(LDLIBS)

$(NAME_CLI):    $(OBJ_CLI)
	$(CXX) -o $(NAME_CLI) $(OBJ_CLI) $(LDFLAGS) $(LDLIBS)

clean:
	$(RM) $(OBJ_SERVER) $(OBJ_CLI)
	$(RM) *.gcno *.gcda

fclean: clean
	$(RM) $(NAME_SERVER) $(NAME_CLI)
	$(RM) $(TEST_NAME)

re: fclean all

tests_run:
	@$(RM) *.gcda *.gcno 
	$(CXX) -o $(TEST_NAME) $(SRC_TESTS) $(SRC_SERVER) $(SRC_CLI) \
		$(CXXFLAGS) -DUNIT_TEST --coverage -lcriterion $(LDFLAGS) $(LDLIBS)
	./$(TEST_NAME)
	gcovr --exclude tests/
	gcovr --exclude tests/ --branches

.PHONY: all clean fclean re tests_run