NAME	= ircserv

CXX	= c++
INCLUDES = $(shell find $(INC_DIR) -type d)
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g $(addprefix -I, $(INCLUDES))
LDLIBS		= -lssl -lcrypto

RM		= rm -f
RMDIR	= rm -rf

SRC_DIR = src
OBJ_DIR = objs
INC_DIR = includes

SRCS	= main.cpp \
			server/Server.cpp server/PacketHandler.cpp \
			server/Channel.cpp \
			server/commands/CommandBase.cpp \
			server/commands/InviteCommand.cpp \
			server/commands/JoinCommand.cpp \
			server/commands/KickCommand.cpp \
			server/commands/ModeCommand.cpp \
			server/commands/NickCommand.cpp \
			server/commands/PartCommand.cpp \
			server/commands/PrivmsgCommand.cpp \
			server/commands/TopicCommand.cpp \
			server/commands/UserCommand.cpp \
			client/Client.cpp \
			Utils.cpp \
			parsing/Identification.cpp

SRCS	:= $(addprefix $(SRC_DIR)/, $(SRCS))

OBJ		= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

DIRS	= $(sort $(dir $(OBJ)))

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME) $(LDLIBS)

$(DIRS):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(DIRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

all: $(NAME)

clean:
	$(RMDIR) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

debug:
	@echo $(INCLUDES)

.PHONY: all clean fclean re debug
