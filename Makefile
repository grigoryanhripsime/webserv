RED     = \033[1;31m
GREEN   = \033[1;32m
YELLOW  = \033[1;33m
RESET   = \033[0;37m

CPP     = c++ -std=c++98 -g3 -fsanitize=address
CFLAGS  = -Wall -Wextra -Werror

SRCSPATH = src
OBJSPATH = objs

# Find all .cpp files inside src/
SRCS = $(shell find $(SRCSPATH) -type f -name "*.cpp")

# Convert source files to object files in objs/ folder
OBJS = $(patsubst $(SRCSPATH)/%.cpp, $(OBJSPATH)/%.o, $(SRCS))

# Find all subdirectories in src/ and add them as include paths (-I)
INCLUDE_DIRS = $(shell find $(SRCSPATH) -type d)
CFLAGS += $(addprefix -I, $(INCLUDE_DIRS))

NAME = webserv

all: $(NAME)

$(NAME): $(OBJS)
	@$(CPP) $(OBJS) -o $(NAME)
	@echo "$(GREEN) Executable file has been created $(RESET)"

# Rule to compile .cpp files into .o files inside objs/
$(OBJSPATH)/%.o: $(SRCSPATH)/%.cpp | create_obj_dirs
	@$(CPP) $(CFLAGS) -c $< -o $@
	@echo "$(YELLOW) Compiled: $< -> $@ $(RESET)"

# Ensure the necessary folders exist
create_obj_dirs:
	@mkdir -p $(OBJSPATH)/mnacac
	@mkdir -p $(OBJSPATH)/Loggers
	@mkdir -p $(OBJSPATH)/Configs
	@mkdir -p $(OBJSPATH)/Directives
	@mkdir -p $(OBJSPATH)/Sockets
	@mkdir -p $(OBJSPATH)/CGI


clean:
	@rm -rf $(OBJSPATH)
	@echo "$(RED) Object files have been deleted $(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED) Executable file has been deleted $(RESET)"

re: fclean all

.PHONY: all clean fclean re create_obj_dirs
