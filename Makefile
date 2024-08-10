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

DEPS := deps/vector/libvector-static.a

.PHONY: clean_all clean clean_vector

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

# makes dep objs too
deps/vector/libvector-static.a:
	@printf "$(RED)creating vector$(NONE)\n"
	mkdir -p dep_objs
	cd deps/vector && \
	cmake . && \
	make && \
	rm -rf vector-example vector-test
	cd dep_objs && \
	ar x ../deps/vector/libvector-static.a

clean_vector:
	@printf "\033[0;31mcleaning vector\033[0m\n"
	cd deps/vector && git clean -x -d -f

clean_all: clean clean_vector