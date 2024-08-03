NAME = ircserv

CC = c++

CFLAGS = -std=c++98

GREEN = \033[1;33m
RESET = \033[0m

SRC			=	main.cpp src/Server.cpp src/Client.cpp src/Channel.cpp \
				src/Bot.cpp src/BotReq.cpp src/ChannelReq.cpp \
				src/ClientReq.cpp \
				src/ServUtils.cpp src/Commands.cpp src/CommandUtils.cpp\

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME) : $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	@echo "$(GREEN) Compiling: $< $(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJ)

fclean: clean
	@rm -rf $(NAME)
	rm -rf log.log
	rm -rf error.log
	@echo "$(GREEN)\ircserv Objects Deleted!\n$(RESET)"

re: fclean all

.PHONY: all clean fclean re
