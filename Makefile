#******************************************************************************#
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teando <teando@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/12 19:37:29 by teando            #+#    #+#              #
#    Updated: 2025/02/07 20:09:07 by teando           ###   ########.fr        #
#                                                                              #
#******************************************************************************#

NAME = philo
ROOT_DIR = .
SUBMIT_UTIL_DIR = submit_util
MANDATORY_DIR = mandatory
BONUS_DIR = bonus
SUBMIT_DIR = $(NAME)

all: mandatory

mandatory:
	$(MAKE) -C $(MANDATORY_DIR)
	mv $(MANDATORY_DIR)/$(NAME) $(ROOT_DIR)

bonus:
	$(MAKE) -C $(BONUS_DIR)
	mv $(BONUS_DIR)/$(NAME) $(ROOT_DIR)

submit: fclean
	mkdir -p $(SUBMIT_DIR)
	cp $(SUBMIT_UTIL_DIR)/Makefile.template $(SUBMIT_DIR)/Makefile
	cp $(MANDATORY_DIR)/src/*.c $(SUBMIT_DIR)/
	cp $(MANDATORY_DIR)/inc/*.h $(SUBMIT_DIR)/
	for file in $(BONUS_DIR)/src/*.c; do \
		filename=$$(basename $$file .c); \
		cp $$file $(SUBMIT_DIR)/$${filename}_bonus.c; \
	done
	for file in $(BONUS_DIR)/inc/*.h; do \
		filename=$$(basename $$file .h); \
		cp $$file $(SUBMIT_DIR)/$${filename}_bonus.h; \
	done

clean:
	$(MAKE) -C $(MANDATORY_DIR) clean
	$(MAKE) -C $(BONUS_DIR) clean
	rm -rf $(SUBMIT_DIR)

fclean:
	rm -f $(ROOT_DIR)/$(NAME)
	$(MAKE) -C $(MANDATORY_DIR) fclean
	$(MAKE) -C $(BONUS_DIR) fclean
	rm -rf $(SUBMIT_DIR)

re: fclean all

norm:
	$(MAKE) -C $(MANDATORY_DIR) norm
	$(MAKE) -C $(BONUS_DIR) norm

.PHONY: all mandatory bonus submit clean fclean re
