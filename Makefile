SRCS = $(wildcard ./src/*.cpp) $(wildcard ./src/*/*.cpp)
OBJS = $(SRCS:.cpp=.o)
CPP = g++
CPPFLAGS = -Wall -Wextra -Wpedantic
SFMLFLAGS = -l sfml-window -l sfml-graphics -l sfml-system

# Style
ifeq ($(OS), Windows_NT)
    DEF =
    BOLD =
    GREEN =
else
    ifneq ($(shell tput setaf 1 2>/dev/null),)
        DEF = \033[0m
        BOLD = \033[1m
        GREEN = \033[32m
    endif
endif

.PHONY: all clean run-packer

# Rules
all: prepare set-default-flags $(OBJS) link clean
set-default-flags:
	@echo "$(BOLD)$(GREEN)---> OPTIMIZED COMPILATION$(DEF)"
	$(eval CPPFLAGS += -Ofast -s)
	$(eval SFMLFLAGS = -l sfml-window -l sfml-graphics -l sfml-system)

release: prepare set-release-flags $(OBJS) link clean
set-release-flags:
	@echo "$(BOLD)$(GREEN)---> RELEASE COMPILATION$(DEF)"
	$(eval CPPFLAGS += -Ofast -s -static -static-libgcc -static-libstdc++ -DSFML_STATIC)
	$(eval SFMLFLAGS = -l sfml-graphics-s -l sfml-window-s -l sfml-system-s -l freetype -l harfbuzz -l graphite2 -l png -l bz2 -l z -l brotlidec -l brotlicommon -l opengl32 -l winmm -l gdi32 -lole32 -lusp10 -l Rpcrt4)

debug: prepare set-debug-flags $(OBJS) link clean
set-debug-flags:
	@echo "$(BOLD)$(GREEN)---> DEBUG COMPILATION$(DEF)"
	$(eval CPPFLAGS += -g3)
	$(eval SFMLFLAGS = -l sfml-window -l sfml-graphics -l sfml-system)

# Prepare
prepare:
	@echo "$(BOLD)$(GREEN)---> PREPARE$(DEF)"
	$(RM) -r build
	mkdir build
	cp src/anon.ttf build/anon.ttf

# Compile
$(OBJS): %.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $<

# Link
link: print-link packer bench generate
print-link:
	@echo "$(BOLD)$(GREEN)---> LINKING$(DEF)"
bench: benchmark.o packer.o instance_gen.o
	$(CPP) $(CPPFLAGS) $^ -o $@
	mv -f $@ build
generate: generate.o instance_gen.o
	$(CPP) $(CPPFLAGS) $^ -o $@
	mv -f $@ build
packer: solve_input.o packer.o visualizer.o
	$(CPP) $(CPPFLAGS) $^ -o $@ $(SFMLFLAGS)
	mv -f $@ build

# Clean
clean:
	@echo "$(BOLD)$(GREEN)---> CLEANING$(DEF)"
	$(RM) *.o
