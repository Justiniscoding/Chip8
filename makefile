NAME = ./Chip8
FLAGS = -I ./include -std=c++17

main:
	clang++ -o $(NAME) $(FLAGS) ./src/**
	$(NAME)
