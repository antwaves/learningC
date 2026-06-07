CC=clang
NAME=main

all: $(NAME).exe run

$(NAME).exe: $(NAME).c
	$(CC) -o $(NAME).exe $(NAME).c

run: $(NAME).exe
	cls && $(NAME).exe

clean:
	del $(NAME).exe