# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lusokol <lusokol@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/07 15:08:42 by macbookpro        #+#    #+#              #
#    Updated: 2022/03/02 19:58:08 by lusokol          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

##########################################

include Makefile.settings.mk

##########################################

.SILENT:

SRCS = $(addprefix $(PATH_SRCS)/,$(SRC))

OBJ_DIR = .obj

OBJ = $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

DEP = $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.d))

MAKEFLAGS += --no-print-directory

BEGIN = 0
NO_RECURS = 0
UNAME_S = $(shell uname)
LINUX = Linux

PROGRESS_BAR = -----------------------------------------
FILLING_CHAR = █

NUM_FILE_BEING_COMPILED = 1
define DRAW_PROGRESS_BAR
	PROGRESS_BAR=$(PROGRESS_BAR) \
	SIZE=$${#PROGRESS_BAR} \
	NB_BAR=`expr $(NUM_FILE_BEING_COMPILED) '*' $$SIZE / $(NB_FILE_TO_COMPILE)`; \
	printf '$(ERASE)$(BLUE)[ $(PROGRESS_BAR)$(_BOLD) ][ %d / %d ]\r[ $(ESC_STOP)' \
		$(NUM_FILE_BEING_COMPILED) $(NB_FILE_TO_COMPILE); \
	for N in `seq $$NB_BAR`; \
		do printf '$(_BOLD)$(BLUE)$(FILLING_CHAR)$(ESC_STOP)'; \
	done
endef

all: $(NAME)

$(OBJ_DIR)/%.o: $(PATH_SRCS)/%.cpp
ifeq ($(NO_RECURS), 0)
ifeq ($(UNAME_S), $(LINUX))
	$(eval NB_FILE_TO_COMPILE ?= $(shell make NO_RECURS=1 --dry-run --debug=b | grep "does not\|Must remake" | grep -o "[\`'].*\.o'" | sort | uniq | wc -l))
else
	$(eval NB_FILE_TO_COMPILE ?= $(shell make NO_RECURS=1 --dry-run --debug=b | grep "does not\|Must remake" | grep -o "\`.*\.o'" | sort | uniq | wc -l))
endif
endif
	if [ '$(BEGIN)' -eq '0' ]; then $(eval BEGIN = 1) make project; printf "\n"; fi;
	mkdir -p $(OBJ_DIR)
	printf "$(ERASE)$(MOVE_UP)$(_ORANGE)$(_ORANGE)┃ Compiling : $(_WHITE)%-30s$(_ORANGE)┃$(ESC_STOP)                      \n" $<
	$(DRAW_PROGRESS_BAR)
	printf "\n"
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I $(INCLUDES) -MMD -MP -c $< -o $@
	if [ '$(NUM_FILE_BEING_COMPILED)' -eq '$(NB_FILE_TO_COMPILE)' ]; then \
		printf "$(MOVE_UP)$(_ORANGE)┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛                      \n$(_R)"; \
		$(DRAW_PROGRESS_BAR); \
		printf '\n'; \
	fi; $(eval NUM_FILE_BEING_COMPILED = $(shell echo $$(($(NUM_FILE_BEING_COMPILED) + 1))))	

project:
	printf "$(_ORANGE)┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n$(_R)"
	printf "$(_ORANGE)┃$(_BOLD) - %24s - 42 Project - $(_R)$(_ORANGE)┃$(_R)\n" $(NAME)
	printf "$(_ORANGE)┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫\n$(_R)"


$(NAME): $(OBJ)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I $(INCLUDES) $(OBJ) -o $(NAME)
	printf "$(_BOLD)$(_GREEN)$(NAME) successfully compiles !$(_R)\n"
	chmod 777 $(NAME)

clean:
	printf "$(_RED)[Clean] Removing .o ...\e[0m\n"
	rm -rf $(OBJ_DIR) $(DEP)

fclean: clean
	printf "$(_RED)[Fclean] Removing $(NAME) ...$(_R)\n"
	rm -rf $(NAME)

re: clean all

run: all
	clear
	./$(NAME) $(PORT) $(PSWD)

rerun: re
	clear
	./$(NAME) $(PORT) $(PSWD)

add_g3:
	$(eval CXXFLAGS = $(CXXFLAGS) -g3)

add_sanitize:
	$(eval CXXFLAGS = $(CXXFLAGS) -fsanitize=address)

debug: add_g3 add_sanitize rerun

leaks:  add_g3 re
ifeq ($(UNAME_S), $(LINUX))
	printf "$(_BOLD)$(_GREEN)$(NAME) execute with valgrind ...$(_R)\n"
	valgrind --leak-check=full --track-origins=yes ./$(NAME) $(PORT) $(PSWD)
else
	printf "$(_RED)Valgrind deprecated on Macos...\e[0m\n"
endif

-include $(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.d))

########################################## colors

_WHITE = \e[97m
_GREEN = \e[38;5;118m
_ORANGE = \e[38;5;208m
_RED = \e[38;5;196m
_BOLD = \e[1m
_R = \e[0m

ESC_SEQ = \033[
BLUE = $(ESC_SEQ)34m
MOVE_UP = $(ESC_SEQ)1A
ERASE = \r$(ESC_SEQ)K
ESC_STOP = $(ESC_SEQ)0m
ERASE_ALL = $(ESC_SEQ)M

##########################################

.PHONY: all clean fclean re project debug leaks