all:
	mkdir -p bin
	gcc src/*.c -ggdb -I include -Wall -Werror-implicit-function-declaration -fsanitize=address -fno-omit-frame-pointer -o bin/kcc
