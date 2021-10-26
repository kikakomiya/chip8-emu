NAME=chip8
CC=clang
CFLAGS=-lSDL2

$(NAME): src/main.c src/cpu.c src/graphics.c
	$(CC) $^ $(CFLAGS) -o $@

.PHONY clean:
	rm $(NAME)
