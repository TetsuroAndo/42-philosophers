# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/07 19:56:42 by teando            #+#    #+#              #
#    Updated: 2025/05/03 19:46:30 by teando           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := philo
CC          := cc
#CFLAGS      := -Wall -Wextra -Werror -O3 -march=native -funroll-loops
CFLAGS      := -Wall -Wextra -Werror -O1 -fsanitize=thread -fPIE -g
RM          := rm -rf
ROOT_DIR    := .
OUT_DIR     := $(ROOT_DIR)/objs
INCS_DIR    := $(ROOT_DIR)/src
IDFLAGS     := -I$(INCS_DIR)

SRCS        := $(shell find src -name "*.c")
OBJS        := $(addprefix $(OUT_DIR)/, $(SRCS:.c=.o))
DEPS        := $(OBJS:.o=.d)

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