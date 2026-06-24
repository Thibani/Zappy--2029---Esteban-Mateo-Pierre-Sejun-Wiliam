##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Server Makefile
##

# Binary
NAME		=	zappy_server

# Compiler & flags
CXX			=	g++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++20
CXXFLAGS	+=	-I./include
LDFLAGS		=

# Directories
SRC_DIR		=	src
OBJ_DIR		=	obj
INC_DIR		=	include

# Sources — adjust paths to match your actual file tree
SRCS		=	$(SRC_DIR)/main.cpp					\
				$(SRC_DIR)/GameLogic/Map.cpp		\
				$(SRC_DIR)/GameLogic/Player.cpp		\
				$(SRC_DIR)/GameLogic/Tile.cpp		\
				$(SRC_DIR)/GameLogic/Game.cpp		\
				$(SRC_DIR)/utils/args.cpp			\
				$(SRC_DIR)/server/server.cpp			\
				$(SRC_DIR)/server/client.cpp			\
				$(SRC_DIR)/server/commandHandler.cpp	\
				$(SRC_DIR)/server/guiCommandHandler.cpp	\
				$(SRC_DIR)/protocol/guiProtocol.cpp

OBJS		=	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Colors
RED			=	\033[0;31m
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
CYAN		=	\033[0;36m
RESET		=	\033[0m

.PHONY: all clean fclean re debug

##
## BUILD RULES
##

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(CYAN)[Server]$(RESET) Linking $(YELLOW)$(NAME)$(RESET)..."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "$(GREEN)[✓]$(RESET) $(NAME) ready."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "$(CYAN)[Server]$(RESET) Compiling $<..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

##
## DEBUG BUILD
##

debug: CXXFLAGS += -g3 -DDEBUG
debug: re

##
## CLEAN RULES
##

clean:
	@echo "$(CYAN)[Server]$(RESET) Removing object files..."
	@$(RM) -r $(OBJ_DIR)
	@echo "$(GREEN)[✓]$(RESET) Object files removed."

fclean: clean
	@echo "$(CYAN)[Server]$(RESET) Removing binary..."
	@$(RM) $(NAME)
	@echo "$(GREEN)[✓]$(RESET) $(NAME) removed."

re: fclean all