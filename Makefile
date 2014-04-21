CPP := gcc -E
CC := gcc
LD := gcc
RM := rm -f
RMDIR := rm -fr

CPPFLAGS +=
CFLAGS += -Wall -Wextra -Werror -ansi -pedantic -ggdb
LDFLAGS +=


DEPDIR=.deps
SRC=unvigenere.c misc.c array.c getopthelp.c charset.c filtered_string.c \
	vigenere.c freq.c mfreq_analysis.c kasiski.c cracker.c
OBJS=$(subst .c,.o,$(SRC))
DEPS=$(patsubst %.c,$(DEPDIR)/%.d,$(SRC))
BIN=unvigenere


.PHONY: all
all: $(BIN)

$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<

$(sort $(dir $(DEPS))):
	mkdir -p $@

.SECONDEXPANSION:
$(DEPDIR)/%.d: %.c Makefile | $$(dir $$@)
	$(CC) -M $(CPPFLAGS) $< | sed 's,\(.*\)\.o:,\1.o $@:,g' > $@

.PHONY: clean mrproper
clean:
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RMDIR) $(DEPDIR)

mrproper: clean
	$(RM) $(BIN)


ifeq ($(MAKECMDGOALS),)
    NONREMOVEGOALS = all
else
    NONREMOVEGOALS = $(filter-out clean mrproper,$(MAKECMDGOALS))
endif

ifneq ($(NONREMOVEGOALS),)
    -include $(DEPS)
endif
