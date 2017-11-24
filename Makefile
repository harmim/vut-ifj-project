# Author: Dominik Harmim <xharmi00@stud.fit.vutbr.cz>

PACK = xharmi00

IS_IT_OK_DIR = is_it_ok_test
IS_IT_OK_SCRIPT = ./tests/is_it_ok.sh

DOC_DIR = doc
DOC = dokumentace.pdf

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror

EXECUTABLE = ifj17_compiler
SRCS = *.c
OBJS = $(shell $(CC) $(CFLAGS) -MM $(SRCS) | grep ':' | cut -d ':' -f1 | tr '\n' ' ')


.PHONY: all
all: $(EXECUTABLE) clean_depdir


$(EXECUTABLE): $(OBJS)


.PHONY: pack
pack: clean $(PACK).tgz


$(PACK).tgz: $(DOC)
	tar -czf $@ *.h *.c Makefile rozdeleni $^


.PHONY: clean_pack
clean_pack:
	rm -f $(PACK).tgz


$(DOC):
	cd $(DOC_DIR) && make
	cp $(DOC_DIR)/$(DOC) $(DOC)


.PHONY: clean_doc
clean_doc:
	rm -f $(DOC)
	cd $(DOC_DIR) && make clean


.PHONY: is_it_ok
is_it_ok: $(PACK).tgz $(IS_IT_OK_SCRIPT) clean_is_it_ok
	chmod +x $(IS_IT_OK_SCRIPT)
	$(IS_IT_OK_SCRIPT) $(PACK).tgz $(IS_IT_OK_DIR)


.PHONY: clean_is_it_ok
clean_is_it_ok:
	rm -rf $(IS_IT_OK_DIR)


.PHONY: clean
clean: clean_pack clean_is_it_ok clean_depdir clean_doc
	rm -rf $(EXECUTABLE) *.o *.out *.dSYM/ log/


# #################### Auto-Dependency Generation #####################
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) > /dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

.PHONY: clean_depdir
clean_depdir:
	rm -rf $(DEPDIR)

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
