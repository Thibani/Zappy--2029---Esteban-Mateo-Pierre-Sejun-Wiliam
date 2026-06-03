##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## Root Makefile
##

# Binaries
SERVER_BIN	=	zappy_server
GUI_BIN		=	zappy_gui
AI_BIN		=	zappy_ai

# Sub-directories
SERVER_DIR	=	server
GUI_DIR		=	gui
AI_DIR		=	ai

# Colors
RED		=	\033[0;31m
GREEN	=	\033[0;32m
YELLOW	=	\033[0;33m
CYAN	=	\033[0;36m
RESET	=	\033[0m

.PHONY: all server gui ai clean fclean re help \
        clean_server clean_gui clean_ai \
        fclean_server fclean_gui fclean_ai \
        tests test

##
## DEFAULT RULES
##

all: zappy_server zappy_gui zappy_ai

zappy_server:
	@echo "$(CYAN)[Zappy]$(RESET) Building $(YELLOW)$(SERVER_BIN)$(RESET)..."
	@$(MAKE) -C $(SERVER_DIR) --no-print-directory
	@cp $(SERVER_DIR)/$(SERVER_BIN) ./$(SERVER_BIN)
	@echo "$(GREEN)[✓]$(RESET) $(SERVER_BIN) built successfully."

zappy_gui:
	@echo "$(CYAN)[Zappy]$(RESET) Building $(YELLOW)$(GUI_BIN)$(RESET)..."
	@$(MAKE) -C $(GUI_DIR) --no-print-directory
	@cp $(GUI_DIR)/$(GUI_BIN) ./$(GUI_BIN)
	@echo "$(GREEN)[✓]$(RESET) $(GUI_BIN) built successfully."

zappy_ai:
	@echo "$(CYAN)[Zappy]$(RESET) Building $(YELLOW)$(AI_BIN)$(RESET)..."
	@$(MAKE) -C $(AI_DIR) --no-print-directory
	@cp $(AI_DIR)/$(AI_BIN) ./$(AI_BIN)
	@echo "$(GREEN)[✓]$(RESET) $(AI_BIN) built successfully."

##
## TEST RULES
##
tests:
	@echo "[Tests] Installing Criterion if needed..."
	@which dpkg > /dev/null && sudo apt-get install -y libcriterion-dev 2>/dev/null || true
	@$(MAKE) -C server/tests all
	@echo "[Tests] Running server tests..."
	@./server/tests/test_args --verbose
	@./server/tests/test_client --verbose

## Alias — CI calls 'make test' (singular)
test: tests

##
## CLEAN RULES
##

clean_zappy_server:
	@echo "$(CYAN)[Zappy]$(RESET) Cleaning $(SERVER_DIR)..."
	@$(MAKE) -C $(SERVER_DIR) clean --no-print-directory

clean_zappy_gui:
	@echo "$(CYAN)[Zappy]$(RESET) Cleaning $(GUI_DIR)..."
	@$(MAKE) -C $(GUI_DIR) clean --no-print-directory

clean_zappy_ai:
	@echo "$(CYAN)[Zappy]$(RESET) Cleaning $(AI_DIR)..."
	@$(MAKE) -C $(AI_DIR) clean --no-print-directory

clean: clean_zappy_server clean_zappy_gui clean_zappy_ai
	@echo "$(GREEN)[✓]$(RESET) All object files removed."

##
## FCLEAN RULES
##

fclean_zappy_server:
	@echo "$(CYAN)[Zappy]$(RESET) Full clean $(SERVER_DIR)..."
	@$(MAKE) -C $(SERVER_DIR) fclean --no-print-directory
	@$(RM) ./$(SERVER_BIN)

fclean_zappy_gui:
	@echo "$(CYAN)[Zappy]$(RESET) Full clean $(GUI_DIR)..."
	@$(MAKE) -C $(GUI_DIR) fclean --no-print-directory
	@$(RM) ./$(GUI_BIN)

fclean_zappy_ai:
	@echo "$(CYAN)[Zappy]$(RESET) Full clean $(AI_DIR)..."
	@$(MAKE) -C $(AI_DIR) fclean --no-print-directory
	@$(RM) ./$(AI_BIN)

fclean: fclean_zappy_server fclean_zappy_gui fclean_zappy_ai
	@$(MAKE) -C server/tests fclean --no-print-directory
	@echo "$(GREEN)[✓]$(RESET) All binaries and object files removed."
##
## RE RULES
##

re: fclean all

##
## HELP
##

help:
	@echo ""
	@echo "$(CYAN)Zappy — Root Makefile$(RESET)"
	@echo ""
	@echo "$(YELLOW)Usage:$(RESET)"
	@echo "  make [target]"
	@echo ""
	@echo "$(YELLOW)Targets:$(RESET)"
	@echo "  $(GREEN)all$(RESET)           Build all three binaries (server, gui, ai)"
	@echo "  $(GREEN)server$(RESET)        Build $(SERVER_BIN) from ./$(SERVER_DIR)/"
	@echo "  $(GREEN)gui$(RESET)           Build $(GUI_BIN) from ./$(GUI_DIR)/"
	@echo "  $(GREEN)ai$(RESET)            Build $(AI_BIN) from ./$(AI_DIR)/"
	@echo ""
	@echo "  $(GREEN)clean$(RESET)         Remove object files from all sub-projects"
	@echo "  $(GREEN)clean_server$(RESET)  Remove object files from server only"
	@echo "  $(GREEN)clean_gui$(RESET)     Remove object files from gui only"
	@echo "  $(GREEN)clean_ai$(RESET)      Remove object files from ai only"
	@echo ""
	@echo "  $(GREEN)fclean$(RESET)        Remove all binaries and object files"
	@echo "  $(GREEN)fclean_server$(RESET) Full clean server only"
	@echo "  $(GREEN)fclean_gui$(RESET)    Full clean gui only"
	@echo "  $(GREEN)fclean_ai$(RESET)     Full clean ai only"
	@echo ""
	@echo "  $(GREEN)re$(RESET)            fclean + all"
	@echo "  $(GREEN)help$(RESET)          Show this message"
	@echo ""