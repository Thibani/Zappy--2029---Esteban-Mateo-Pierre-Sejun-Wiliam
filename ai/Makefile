##
## EPITECH PROJECT, 2025
## Zappy
## File description:
## AI Makefile (Python)
##

# Binary name (shell wrapper placed at the root of ai/)
NAME		=	zappy_ai

# Python interpreter
PYTHON		=	python3
VENV		=	.venv
PIP			=	$(VENV)/bin/pip

# Entry point
MAIN		=	src/main.py

# Tests
TESTS_DIR	=	tests
TESTS_ENV	=	PYTHONPATH=$(CURDIR)/src:$(CURDIR)/tests

# Colors
RED			=	\033[0;31m
GREEN		=	\033[0;32m
YELLOW		=	\033[0;33m
CYAN		=	\033[0;36m
RESET		=	\033[0m

.PHONY: all clean fclean re tests

##
## BUILD RULES
##

all: $(NAME)

$(VENV)/bin/python:
	@echo "$(CYAN)[AI]$(RESET) Creating virtual environment..."
	@$(PYTHON) -m venv $(VENV)
	@echo "$(GREEN)[✓]$(RESET) Virtual environment ready."

$(NAME): $(VENV)/bin/python
	@if [ -f requirements.txt ]; then \
		echo "$(CYAN)[AI]$(RESET) Installing dependencies..."; \
		$(PIP) install --quiet -r requirements.txt; \
		echo "$(GREEN)[✓]$(RESET) Dependencies installed."; \
	fi
	@echo "$(CYAN)[AI]$(RESET) Generating $(YELLOW)$(NAME)$(RESET) wrapper..."
	@printf '#!/bin/sh\nexec "$(CURDIR)/$(VENV)/bin/python" "$(CURDIR)/$(MAIN)" "$$@"\n' > $(NAME)
	@chmod +x $(NAME)
	@echo "$(GREEN)[✓]$(RESET) $(NAME) ready."

##
## TEST RULES
##
## PYTHONPATH expose src/ et tests/ aux scripts de test,
## ce qui permet d'écrire "from parser import ..." sans préfixe.
##

tests:
	@echo "$(CYAN)[AI]$(RESET) Running tests..."
	@$(TESTS_ENV) $(PYTHON) $(TESTS_DIR)/run.py
	@echo "$(GREEN)[✓]$(RESET) Tests done."

##
## CLEAN RULES
##

clean:
	@echo "$(CYAN)[AI]$(RESET) Removing __pycache__ directories..."
	@find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null || true
	@find . -name "*.pyc" -delete 2>/dev/null || true
	@echo "$(GREEN)[✓]$(RESET) Cache files removed."

fclean: clean
	@echo "$(CYAN)[AI]$(RESET) Removing binary wrapper and virtualenv..."
	@$(RM) $(NAME)
	@$(RM) -r $(VENV)
	@echo "$(GREEN)[✓]$(RESET) $(NAME) and $(VENV) removed."

re: fclean all