SRCS = $(wildcard ./src/*.cpp) $(wildcard ./src/*/*.cpp)
OBJS = $(SRCS:.cpp=.o)
CPP = g++
CPPFLAGS = -Wall -Wextra -Wpedantic
SFMLFLAGS = -l sfml-window -l sfml-graphics -l sfml-system

# Style
ifneq ($(shell tput setaf 1 2> /dev/null),)
	DEF = \033[0m
	BOLD = \033[1m
	UBOLD = \033[21m
	CUR = \033[3m
	UL = \033[4m
	GREEN = \033[32m
	MAGENTA = \033[35m
endif

# Rules
all: prepare set-default-flags $(OBJS) link clean
set-default-flags:
	@echo "\n$(BOLD)$(GREEN)OPTIMIZED COMPILATION$(DEF)\n"
	$(eval CPPFLAGS += -Ofast -s -static)

debug: prepare set-debug-flags $(OBJS) link clean
set-debug-flags:
	@echo "\n$(BOLD)$(GREEN)DEBUG COMPILATION$(DEF)\n"
	$(eval CPPFLAGS += -g3)

# Prepare
prepare:
	@echo "\n$(BOLD)$(GREEN)PREPARE$(DEF)\n"
	$(RM) -r build
	mkdir build
	cp src/anon.ttf build/anon.ttf

# Compile
$(OBJS): %.o: %.cpp
	$(CPP) $(CPPFLAGS) -c $<

# Link
link: print-link packer bench generate
print-link:
	@echo "\n$(BOLD)$(GREEN)LINKING$(DEF)\n"
bench: benchmark.o packer.o
	$(CPP) $^ -o $@
	mv -f $@ build
generate: generate.o
	$(CPP) $^ -o $@
	mv -f $@ build
packer: solve_input.o packer.o visualizer.o
	$(CPP) $^ -o $@ $(SFMLFLAGS)
	mv -f $@ build

# Clean
clean:
	@echo "\n$(BOLD)$(GREEN)CLEANING$(DEF)\n"
	$(RM) *.o
