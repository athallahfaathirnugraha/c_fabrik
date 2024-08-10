NAME := fabrik

# colors

RED := \033[0;31m
NONE := \033[0m

# colors

INCLUDE := -I include -I deps/vector

ifeq ($(RELEASE),1)
CFLAGS := $(INCLUDE)
TAR_DIR := release
else
CFLAGS := $(INCLUDE) -DDEBUG -g
TAR_DIR := debug
endif

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,obj/$(TAR_DIR)/%.o,$(SRCS))
DEPREQ := $(patsubst src/%.c,depreq/$(TAR_DIR)/%.d,$(SRCS))

DEPS := 

.PHONY: clean_all clean

out/$(TAR_DIR)/lib$(NAME).a: $(DEPREQ) $(OBJS) $(DEPS)
	@printf "$(RED)creating library ($@)$(NONE)\n"
	$(eval DEP_OBJS := $(foreach lib,$(DEPS),dep_objs/$(shell ar t $(lib) | sed -n '2p')))
	mkdir -p $(dir $@)
	ar rcs $@ $(DEP_OBJS) $(OBJS)

depreq/$(TAR_DIR)/%.d: src/%.c
	@printf "$(RED)creating dependency file $@$(NONE)\n"
	mkdir -p $(dir $@)
	rm -rf $@
	touch $@
	printf "$@ obj/$(patsubst depreq/%,%,$(dir $@))" >> $@
	gcc $(CFLAGS) -MM $< >> $@

-include $(DEPREQ)

obj/$(TAR_DIR)/%.o: src/%.c depreq/$(TAR_DIR)/%.d
	@printf "$(RED)creating object file $@$(NONE)\n"
	mkdir -p $(dir $@)
	gcc -c $(CFLAGS) $< -o $@

clean:
	@echo "cleaning $(NAME)"
	rm -rf depreq obj out

clean_all: clean