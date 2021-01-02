link: compile
	gcc parse.o cstring.o transtlate.o main.o -o karga  `pkg-config --libs gtk+-3.0 webkit2gtk-4.0` -lcurl -g  -Wall 
	rm parse.o
	rm cstring.o
	rm transtlate.o
	rm main.o

compile: 
	gcc -c parse.c -o parse.o -g -Wall
	gcc -c cstring.c -o cstring.o -g -Wall
	gcc -c transtlate.c -o transtlate.o `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`  -g -Wall
	gcc -c main.c -o main.o `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`  -Wall --std=c99 -g

clean:
	rm karga
	rm parse.o
	rm cstring.o
	rm transtlate.o
	rm main.o