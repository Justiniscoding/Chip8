NAME = ./Chip8
FLAGS = -I ./include -std=c++17
SDL_FLAGS = $(shell pkg-config --libs sdl3) $(shell pkg-config --cflags sdl3)
IMGUI_FLAGS = -I ./imgui ./imgui/*.cpp ./imgui/backends/imgui_impl_sdl3.cpp ./imgui/backends/imgui_impl_sdlrenderer3.cpp

main:
	# /opt/homebrew/opt/gcc/bin/g++-16 -o $(NAME) $(FLAGS) ./src/** $(SDL_FLAGS) -Wall -Wextra -Wimplicit-fallthrough
	clang++ -o $(NAME) $(FLAGS) ./src/** $(SDL_FLAGS) $(IMGUI_FLAGS) -Wall -Wextra -Wimplicit-fallthrough
	$(NAME)

debug:
	/opt/homebrew/opt/gcc/bin/g++-16 -g -gdwarf-4 -o $(NAME) $(FLAGS) ./src/** $(SDL_FLAGS) 
	lldb $(NAME)
