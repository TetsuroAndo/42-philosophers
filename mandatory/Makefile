# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/07 19:56:42 by teando            #+#    #+#              #
#    Updated: 2025/05/03 20:15:20 by teando           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        := philo
NAME_BONUS  := philo_bonus
CC          := cc
#CFLAGS      := -Wall -Wextra -Werror -O3 -march=native -funroll-loops
CFLAGS      := -Wall -Wextra -Werror -O1 -fsanitize=thread -fPIE -g
RM          := rm -rf
ROOT_DIR    := .
OUT_DIR     := $(ROOT_DIR)/obj
INCS_DIR    := $(ROOT_DIR)/src
IDFLAGS     := -I$(INCS_DIR)

SRCS        := $(shell find . -name "*.c" -not -name "*_bonus.c")
SRCS_BONUS  := $(shell find . -name "*_bonus.c")
OBJS        := $(addprefix $(OUT_DIR)/, $(SRCS:.c=.o))
OBJS_BONUS  := $(addprefix $(OUT_DIR)/, $(SRCS_BONUS:.c=.o))
DEPS        := $(OBJS:.o=.d)
DEPS_BONUS  := $(OBJS_BONUS:.o=.d)

all: $(NAME)

bonus: $(NAME_BONUS)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -pthread $(OBJS) -o $@

$(NAME_BONUS): $(OBJS_BONUS)
	$(CC) $(CFLAGS) -pthread $(OBJS_BONUS) -o $@

$(OUT_DIR)/%.o: $(ROOT_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -MP $(IDFLAGS) -c $< -o $@

clean:
	$(RM) $(OUT_DIR)

fclean: clean
	$(RM) $(NAME) $(NAME_BONUS)

re: fclean all

norm:
	@norminette $(SRCS) $(INCS_DIR)

.PHONY: all clean fclean re norm

-include $(DEPS)