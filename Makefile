#CMDS
RM= rm -rf
MKDIR= mkdir -p

#DIRECTORIES
HEADERS=	webserv_code/headers
INCLUDES=	webserv_code/includes
BUILD=		webserv_code/build
SRCS_DIR=	webserv_code/sources
OBJS_DIR=	$(BUILD)/objects
DEPS_DIR=	$(BUILD)/dependencies
DIRS= 		$(BUILD) $(DEPS_DIR) $(OBJS_DIR)

#FILES
SRCS=		$(wildcard $(SRCS_DIR)/*.cpp)
OBJS=		$(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)
DEPS=		$(OBJS:$(OBJS_DIR)/%.o=$(DEP_DIR)/%.d)
CONFIG=		webserv_code/config/default.conf

#EXEC
NAME=	$(BUILD)/webserv
ARGS=	$(CONFIG)

#FLAGS
CPPFLAGS=	-I $(HEADERS) -I $(INCLUDES) -Wall -Wextra -Werror -std=c++98
DEPFLAGS=	-MMD -MP -MT $(OBJS_DIR)/$*.o -MF $(DEPS_DIR)/$*.d

#COMPILER
CPP= 		c++

#RULES
all: $(NAME)

$(NAME): $(DIRS) $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(OBJS): $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(CPP) $(CPPFLAGS) $(DEPFLAGS) -c $^ -o $@
$(DIRS):
	$(MKDIR) $(DIRS)

clean:
	$(RM) $(OBJS_DIR) $(DEPS_DIR)

fclean:
	$(RM) $(BUILD)

re: fclean all

run: re
	./$(NAME) $(ARGS)

-include $(DEPS)
