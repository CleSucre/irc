NAME	= ircserv

CXX	= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -Iincludes -g

RM		= rm -f
RMDIR	= rm -rf

SRC_DIR = src
OBJ_DIR = objs
INC_DIR = includes

SRCS	= main.cpp

SRCS	:= $(addprefix $(SRC_DIR)/, $(SRCS))

OBJ		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(NAME)

clean:
	$(RMDIR) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
