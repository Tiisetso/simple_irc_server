NAME := ircserv

CXX := c++
CXXFLAGS := -Wall -Wextra -Werror -std=c++20
CPPFLAGS := -Iinclude

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
$(SRC_DIR)/main.cpp \
$(SRC_DIR)/Server.cpp \
$(SRC_DIR)/Port.cpp

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

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

.PHONY: all clean fclean re
