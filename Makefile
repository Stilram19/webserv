NAME=	build/webserv

#CMDS
RM= rm -rf
CPP= c++
MKDIR= mkdir -p

#DIRECTORIES
BUILD=		webserv_code/build
EXEC_DIR=	webserv_code/exec_dir
SRCS_DIR=	webserv_code/sources
OBJS_DIR=	${BUILD}/objects
DEPS_DIR=	${BUILD}/dependencies
DIRS= 		${BUILD} ${DEPS_DIR} ${EXEC_DIR} ${OBJS_DIR}

#FILES
SRCS=		${wildcard ${SRCS_DIR}/*.cpp}
OBJS=		${SRCS:${SRCS_DIR}/%.cpp=${OBJS_DIR}/%.o}
DEPS=		${OBJS:${OBJS_DIR}/%.o=${DEP_DIR}/%.d}

#FLAGS
CPPFLAGS=	-Wall -Wextra -Werror -std=c++98
DEPFLAGS=	-MMD -MP

#COMPILER
CPP= 		c++

#RULES
all: ${NAME}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp
	${CPP} ${CPPFLAGS} ${DEPFLAGS} -c $< -o $@

${NAME}: ${DIRS} ${OBJS}
	${CPP} ${CPPFLAGS} ${OBJS} -o ${NAME}

${DIRS}:
	${MKDIR} ${DIRS}

clean:
	${RM} ${BUILD}

fclean: clean
	${RM} ${EXEC_DIR}

re: fclean all

-include ${DEPS}
