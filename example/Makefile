LIBNAME := fabrik

# colors

RED := \033[0;31m
NONE := \033[0m

# colors

INCLUDE := -I deps/raylib/src -I ../include

DEPS := deps/raylib/src/libraylib.a

ifeq ($(RELEASE),1)
CFLAGS := $(INCLUDE)
TAR_DIR := release
DEPS += ../out/release/lib$(LIBNAME).a
else
CFLAGS := $(INCLUDE) -DDEBUG -g
TAR_DIR := debug
DEPS += ../out/debug/lib$(LIBNAME).a
endif

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,obj/$(TAR_DIR)/%.o,$(SRCS))
DEPREQ := $(patsubst src/%.c,depreq/$(TAR_DIR)/%.d,$(SRCS))

LFLAGS := -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

.PHONY: clean_all clean clean_raylib

out/$(TAR_DIR)/example: $(DEPREQ) $(OBJS) $(DEPS)
	@printf "$(RED)creating executable ($@)$(NONE)\n"
	mkdir -p $(dir $@)
	gcc $(OBJS) $(LFLAGS) $(DEPS) -o $@

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

deps/raylib/src/libraylib.a:
	@printf "$(RED)creating raylib$(NONE)\n"
	cd deps/raylib/src && \
	make

clean_raylib:
	@printf "$(RED)cleaning raylib$(NONE)\n"
	cd deps/raylib && \
	git clean -d -x -f && \
	git reset --hard

clean:
	@printf "$(RED)cleaning example$(NONE)\n"
	rm -rf depreq obj out

clean_all: clean clean_raylib