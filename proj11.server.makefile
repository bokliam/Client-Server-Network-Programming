proj11.server: proj11.server.o
	g++ proj11.server.o -o proj11.server

proj11.server.o: proj11.server.c
	g++ -Wall -c -g proj11.server.c 
	
clean:
	rm -f proj11.server.o
	rm -f proj11.server


