RED		= \033[1;31m
GREEN	= \033[1;32m
YELLOW	= \033[1;33m
RESET	= \033[0;37m
SKY		= \033[1;36m

CPP = c++ -std=c++98 -g3 -fsanitize=address
SRCSPATH = ./src/
INCLPATH = ./headers/
OBJSPATH = ./objs/

SRCS = $(shell find $(SRCSPATH) -name "*.cpp")
OBJS = $(patsubst $(SRCSPATH)%.cpp, $(OBJSPATH)%.o, $(SRCS))

CFLAGS = -Wall -Wextra -Werror  #$(addprefix -I, $(shell find $(INCLPATH) -type d)) 
NAME = webserv

all : $(NAME)

$(NAME) : $(OBJS)
	@$(CPP) $^ -o $@
	@echo "$(GREEN) Executable file has been created $(RESET)"

$(OBJSPATH)%.o : $(SRCSPATH)%.cpp | $(OBJSPATH)
	@$(CPP) $(CFLAGS) -c $< -o $@
	@echo "$(YELLOW) Object file $@ has been created $(RESET)"

$(OBJSPATH):
	@mkdir -p $(OBJSPATH)

clean :
	@rm -rf $(OBJSPATH)
	@echo "$(RED) Object files have been deleted $(RESET)"

fclean : clean
	@rm -f $(NAME)
	@echo "$(RED) Executable file has been deleted $(RESET)"

re : fclean all

.PHONY : all clean fclean re
