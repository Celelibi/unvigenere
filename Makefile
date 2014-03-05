CPP := gcc -E
CC := gcc
LD := gcc
RM := rm -f

CPPFLAGS +=
CFLAGS += -Wall -Wextra -Werror -ansi -pedantic -ggdb
LDFLAGS +=


SRC=unvigenere.c misc.c getopthelp.c
OBJS=$(subst .c,.o,$(SRC))
BIN=unvigenere


$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<


.PHONY: clean mrproper
clean:
	$(RM) $(OBJS)

mrproper: clean
	$(RM) $(BIN)
