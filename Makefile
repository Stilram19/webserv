# .SILENT :

# *************************************************************************** #
#                                    FILES                                    #
# *************************************************************************** #

NOM			= webserv
NAME		= $(EXE_DIR)/$(NOM)
NAME_UP		= $(shell echo $(NOM) | tr '[:lower:]' '[:upper:]')

MAIN_DIR	= webserv_code

# *************WEBSERV_CODE************* #

INCLUDE		= \
	-I $(MAIN_DIR)/headers							\
	-I $(MAIN_DIR)/includes
SRCS		= $(MAIN_DIR)/Sources/main.cpp

# ***************CLIENTS**************** #

SRCS		+= \
	# $(MAIN_DIR)/Sources/MainClient.cpp



# *************************************************************************** # 
#                                   FOLDERS                                   #
# *************************************************************************** #

DEP_DIR = dependencies
EXE_DIR	= executable
OBJ_DIR	= objects

DIRS	= $(DEP_DIR) $(EXE_DIR) $(OBJ_DIR)
MKDIR	= mkdir -p $(DIRS)

# *************************************************************************** #
#                                     CPP                                     #
# *************************************************************************** #

CPP			= c++
CPPFLAGS	= -Wall -Wextra -Werror -g
CPPFLAGS	+= -std=c++98
CPPFLAGS	+= #-fsanitize=address -g3

COMPILE		= $(CPP) $(CPPFLAGS) $(DEPFLAGS) $(INCLUDE)

# *************************************************************************** #
#                                     DEP                                     #
# *************************************************************************** #

DEP_FILES	= $(addprefix $(DEP_DIR)/, $(SRCS:.cpp=.d))

DEPFLAGS	= -MT $@ -MMD -MP -MF $(DEP_DIR)/$*.d

# *************************************************************************** #
#                                     OBJ                                     #
# *************************************************************************** #

OBJ_FILES	= $(addprefix $(OBJ_DIR)/, $(SRCS:.cpp=.o))

# *************************************************************************** # 
#                                   COMPILE                                   #
# *************************************************************************** #

$(OBJ_DIR)/%.o : %.cpp
	mkdir -p $(dir $@)
	mkdir -p $(dir $(DEP_DIR)/$*.d)
	$(COMPILE) -c $< -o $@

# **************************************************************************** # 
#                                    MAKING                                    #
# **************************************************************************** #

all : $(DIRS) $(NAME)

-include $(DEP_FILES)

$(DIRS) :
	$(MKDIR)

$(NAME) : $(OBJ_FILES)
	$(COMPILE) $(OBJ_FILES) -o $(NAME)

clean:
	$(RM) -rf $(DEP_DIR);
	$(RM) -rf $(OBJ_DIR);

fclean : clean
	$(RM) -rf $(EXE_DIR);

re : fclean all

run : all
	./$(NAME) default.conf

.PHONY : all clean fclean re run
