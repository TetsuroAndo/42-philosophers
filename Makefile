# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/12 19:37:29 by teando            #+#    #+#              #
#    Updated: 2025/05/02 08:34:22 by teando           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME 			:= philo
ROOT_DIR 		:= .
RM				:= rm -rf
MANDATORY_DIR 	:= $(ROOT_DIR)/mandatory
BONUS_DIR 		:= $(ROOT_DIR)/bonus
SUBMIT_DIR 		:= $(ROOT_DIR)/$(NAME)

all: mandatory

mandatory:
	$(RM) $(ROOT_DIR)/$(NAME)
	$(MAKE) -C $(MANDATORY_DIR)
	mv $(MANDATORY_DIR)/$(NAME) $(ROOT_DIR)/$(NAME)

bonus:
	$(RM) $(ROOT_DIR)/$(NAME)
	$(MAKE) -C $(BONUS_DIR)
	mv $(BONUS_DIR)/$(NAME) $(ROOT_DIR)/$(NAME)

submit: fclean
	mkdir -p $(SUBMIT_DIR)
	cp $(MANDATORY_DIR)/Makefile $(SUBMIT_DIR)/Makefile
	cp $(MANDATORY_DIR)/src/*.c $(SUBMIT_DIR)/
	cp $(MANDATORY_DIR)/src/*.h $(SUBMIT_DIR)/
	for file in $(BONUS_DIR)/src/*.c; do \
		filename=$$(basename $$file .c); \
		cp $$file $(SUBMIT_DIR)/$${filename}_bonus.c; \
	done
	cp $(BONUS_DIR)/src/philo_bonus.h $(SUBMIT_DIR)/

clean:
	$(MAKE) -C $(MANDATORY_DIR) clean
	$(MAKE) -C $(BONUS_DIR) clean
	$(RM) $(SUBMIT_DIR)

fclean: clean
	$(RM) $(ROOT_DIR)/$(NAME)
	$(MAKE) -C $(MANDATORY_DIR) fclean
	$(MAKE) -C $(BONUS_DIR) fclean

re: fclean all

norm:
	$(MAKE) -C $(MANDATORY_DIR) norm
	$(MAKE) -C $(BONUS_DIR) norm

.PHONY: all mandatory bonus submit clean fclean re
