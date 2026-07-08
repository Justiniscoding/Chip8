NAME = ./Chip8
FLAGS = -I ./include -std=c++17
SDL_FLAGS = $(shell pkg-config --libs sdl3) $(shell pkg-config --cflags sdl3)

main:
	clang++ -o $(NAME) $(FLAGS) ./src/** $(SDL_FLAGS) -Wall -Wextra -Wimplicit-fallthrough
	$(NAME)

debug:
	clang++ -g -gdwarf-4 -o $(NAME) $(FLAGS) ./src/** $(SDL_FLAGS) 
	lldb $(NAME)
