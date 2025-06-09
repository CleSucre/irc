NAME	= ircserv
BOT_NAME = bot

CXX	= c++
INCLUDES = $(shell find $(INC_DIR) -type d)
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g $(addprefix -I, $(INCLUDES))
BOTCXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -Ibot_includes -g

LDLIBS		= -lssl -lcrypto

RM		= rm -f
RMDIR	= rm -rf

SRC_DIR = src
OBJ_DIR = objs
INC_DIR = includes

#========
# Bot specific variables
BOT_SRC_DIR = bot_src
BOT_INC = bot_includes
BOT_SRC = bot_src/main.cpp bot_src/Bot.cpp bot_src/Client.cpp bot_src/Channel.cpp \
			bot_src/CodeMap.cpp
BOT_OBJ_DIR = bot_objs
BOT_OBJ = $(BOT_SRC:$(BOT_SRC_DIR)/%.cpp=$(BOT_OBJ_DIR)/%.o)
#========
SRCS	= main.cpp \
			server/Server.cpp server/PacketHandler.cpp \
			server/Channel.cpp \
			server/commands/CommandBase.cpp \
			server/commands/InviteCommand.cpp \
			server/commands/JoinCommand.cpp \
			server/commands/KickCommand.cpp \
			server/commands/ListCommand.cpp \
			server/commands/ModeCommand.cpp \
			server/commands/NickCommand.cpp \
			server/commands/PassCommand.cpp \
			server/commands/PingCommand.cpp \
			server/commands/PartCommand.cpp \
			server/commands/PrivmsgCommand.cpp \
			server/commands/TopicCommand.cpp \
			server/commands/WhoCommand.cpp \
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

#========
# Bot specific rules
bot: $(BOT_NAME)

$(BOT_NAME): $(BOT_OBJ)
	$(CXX) $(BOTCXXFLAGS) $(BOT_SRC) -o $(BOT_NAME) $(LDLIBS)

$(BOT_OBJ_DIR)/%.o: $(BOT_SRC_DIR)/%.cpp | $(BOT_OBJ_DIR)
	$(CXX) $(BOTCXXFLAGS) -c $< -o $@
$(BOT_OBJ_DIR):
	mkdir -p $(BOT_OBJ_DIR)

cleanbot:
	$(RMDIR) $(BOT_NAME) $(BOT_OBJ_DIR)
#========
all: $(NAME)

clean:
	$(RMDIR) $(OBJ_DIR)

fclean: clean cleanbot
	$(RM) $(NAME)

re: fclean all

debug:
	@echo $(INCLUDES)

.PHONY: all clean fclean re debug bot cleanbot
