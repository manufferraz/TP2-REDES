all:
	gcc -Wall -c common.c
	gcc -Wall client1.c common.o -o client1
	gcc -Wall client.c common.o -o client
	gcc -Wall server.c common.o -o server
	gcc -Wall servermulticlient.c common.o -o servermulticlient