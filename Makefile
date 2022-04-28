# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ebellon <ebellon@student.42lyon.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/03/23 12:34:52 by ebellon           #+#    #+#              #
#    Updated: 2022/03/02 15:48:13 by ebellon          ###   ########lyon.fr    #
#                                                                              #
# **************************************************************************** #

NAME			= sound_visual
FILES			=

INC_FILES		=
INC_PATH		= ./Headers/
INC				= $(addprefix ${INC_PATH}, ${INC_FILES})
SRC_PATH		= ./Srcs/
SRC				= $(addprefix ${SRC_PATH}, ${FILES})

CC				= gcc
OBJS			= ${SRC:.c=.o}
FLAGS			= -Wall -Wextra -O3 -march=native

#LIBRARY
NAME_MINILIBX	= libmlx.dylib
MINILIBX_PATH	= minilibx/
MINILIBX		= $(addprefix ${MINILIBX_PATH}, ${NAME_MINILIBX})

all:			${NAME}

# linking
${NAME}:		lib ${OBJS}
				${CC} ${OBJS} main.c -o ${NAME} ${NAME_MINILIBX} -I ${INC_PATH} $(FLAGS)

%.o: %.c		${INC}
				${CC} -c $< -o $@ -I ${INC_PATH} ${FLAGS}

# source compilation
lib:
				make -C ${MINILIBX_PATH}
				cp ${MINILIBX} .


clean:
				make -C ${MINILIBX_PATH} clean
				${RM} ${OBJS} ${OBJS_BONUS}
				${RM} sound_visual.bmp

fclean:			clean
				${RM} ${NAME}
				${RM} ${NAME_MINILIBX}
				${RM} ${MINILIBX}

re:				fclean all


.PHONY: all clean fclean re bonus lib test