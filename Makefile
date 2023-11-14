all:
	gcc -Wall -c common.c
	gcc -Wall client1.c common.o -o client1
	gcc -Wall client2.c common.o -o client2
	gcc -Wall server1.c common.o -o server1
	gcc -Wall servermulticlient.c common.o -o servermulticlient