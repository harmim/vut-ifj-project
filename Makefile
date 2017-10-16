# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

PACK = xharmi00

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic

EXECUTABLE = ifj17_compiler
SRCS = *.c
OBJS = $(shell $(CC) $(CFLAGS) -MM $(SRCS) | grep ':' | cut -d ':' -f1 | tr '\n' ' ')


.PHONY: all
all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJS)


.PHONY: pack
pack: clean $(PACK).tgz


$(PACK).tgz:
	tar -czf $@ *.h *.c Makefile


.PHONY: clean
clean:
	rm -rf $(EXECUTABLE) *.o *.out *.dSYM/ $(PACK) $(DEPDIR)


# #################### Auto-Dependency Generation #####################
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) > /dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

%.o: %.c
%.o: %.c $(DEPDIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %, $(DEPDIR)/%.d, $(basename $(SRCS))))
# #####################################################################
