# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/07 19:56:42 by teando            #+#    #+#              #
#    Updated: 2025/05/05 01:46:17 by teando           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		:= philo_bonus
CC 			:= cc
CFLAGS		:= -Wall -Wextra -Werror -O3 -march=native -funroll-loops
RM			:= rm -rf
ROOT_DIR	:= .
OUT_DIR		:= $(ROOT_DIR)/obj
INCS_DIR	:= $(ROOT_DIR)
IDFLAGS		:= -I$(INCS_DIR)

SRCS		:= $(addprefix $(ROOT_DIR)/, \
			data.c \
			err.c \
			life.c \
			main.c \
			misc.c \
			)
OBJS		:= $(addprefix $(OUT_DIR)/, $(SRCS:.c=.o))
DEPS		:= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -pthread $(OBJS) -o $@

$(OUT_DIR)/%.o: $(ROOT_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP $(IDFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

norm:
	@norminette $(SRCS) $(INCS_DIR)

.PHONY: all clean fclean re norm

-include $(DEPS)
