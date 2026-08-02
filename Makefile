NAME := ircserv

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++20
CPPFLAGS := -MMD -MP -Iinclude

VFLAGS := -g -O0
SFLAGS := -g -O0 -fsanitize=address -fsanitize=leak -fsanitize=undefined

debug ?= 0

ifeq ($(debug), 1)
CXXFLAGS += $(VFLAGS)
endif

ifeq ($(debug), 2)
CXXFLAGS += $(SFLAGS)
endif

SRC_DIR := src
OBJ_DIR := obj

SRCS := \
$(SRC_DIR)/Cap.cpp \
$(SRC_DIR)/Channel.cpp \
$(SRC_DIR)/Invite.cpp \
$(SRC_DIR)/Join.cpp \
$(SRC_DIR)/Kick.cpp \
$(SRC_DIR)/main.cpp \
$(SRC_DIR)/Message.cpp \
$(SRC_DIR)/Nick.cpp \
$(SRC_DIR)/Parser.cpp \
$(SRC_DIR)/Part.cpp \
$(SRC_DIR)/Ping.cpp \
$(SRC_DIR)/PrivMsg.cpp \
$(SRC_DIR)/Quit.cpp \
$(SRC_DIR)/Registration.cpp \
$(SRC_DIR)/Server.cpp \
$(SRC_DIR)/User.cpp \
$(SRC_DIR)/Utilities.cpp \
$(SRC_DIR)/Validation.cpp \
$(SRC_DIR)/Mode.cpp

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

valgrind:
		@$(MAKE) re debug=1
		valgrind --leak-check=full \
				 --show-leak-kinds=all \
				 --track-origins=yes \
				 ./$(NAME) $(PORT) $(PASSWORD)

-include $(DEPS)
.PHONY: all clean fclean re
