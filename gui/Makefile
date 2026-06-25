##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## GUI Makefile
##

NAME		=	zappy_gui

CXX			=	g++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++20
CXXFLAGS	+=	-I./src
LDFLAGS		=	-lraylib -lm

SRC_DIR		=	src
OBJ_DIR		=	obj

SRCS		=	$(SRC_DIR)/main.cpp								\
				$(SRC_DIR)/Setup.cpp							\
				$(SRC_DIR)/Camera/Camera.cpp					\
				$(SRC_DIR)/Map/Map.cpp							\
				$(SRC_DIR)/Renderer/Renderer.cpp				\
				$(SRC_DIR)/Character/CharacterFactory.cpp		\
				$(SRC_DIR)/Protocol/ServerParser.cpp			\
				$(SRC_DIR)/Network/NetworkClient.cpp			\
				$(SRC_DIR)/Egg/EggFactory.cpp					\
				$(SRC_DIR)/Core/AssetManager.cpp

OBJS		=	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

RED			=	\033[0;31m
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
CYAN		=	\033[0;36m
RESET		=	\033[0m

.PHONY: all clean fclean re debug

all: $(NAME)

$(NAME): $(OBJS)
	@printf "$(CYAN)[GUI]$(RESET) Linking $(YELLOW)$(NAME)$(RESET)...\n"
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@printf "$(GREEN)[OK]$(RESET) $(NAME) ready.\n"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@printf "$(CYAN)[GUI]$(RESET) Compiling $<...\n"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += -g3 -DDEBUG
debug: re

clean:
	@printf "$(CYAN)[GUI]$(RESET) Removing object files...\n"
	@$(RM) -r $(OBJ_DIR)
	@printf "$(GREEN)[OK]$(RESET) Object files removed.\n"

fclean: clean
	@printf "$(CYAN)[GUI]$(RESET) Removing binary...\n"
	@$(RM) $(NAME)
	@printf "$(GREEN)[OK]$(RESET) $(NAME) removed.\n"

re: fclean all