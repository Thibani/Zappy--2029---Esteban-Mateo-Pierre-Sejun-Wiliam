##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Tests Makefile
##
CXX         =   g++
CXXFLAGS    =   -Wall -Wextra -std=c++20 -I../include
LDFLAGS     =   -lcriterion

SRC_DIR     =   .
SERVER_SRC  =   ../src

TESTS       =   test_args test_client

SRCS_COMMON =   $(SERVER_SRC)/utils/args.cpp    \
                $(SERVER_SRC)/server/client.cpp

.PHONY: all clean fclean re test_args test_client

all: $(TESTS)

test_args: test_args.cpp $(SRCS_COMMON)
	@echo "[Tests] Building test_args..."
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test_client: test_client.cpp $(SRCS_COMMON)
	@echo "[Tests] Building test_client..."
	@$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	@$(RM) $(TESTS)

fclean: clean

re: fclean all