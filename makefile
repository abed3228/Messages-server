prog: server.o client.o 
	gcc server.o -o server.exe
	gcc client.o -o client.exe

main.o: server.c client.c
	gcc -Wall -c  server.c
	gcc -Wall -c  client.c

rm:  
	rm *.exe

all: prog

PHONY: clean

clean:
	rm server.o client.o 
	rm server.exe client.exe