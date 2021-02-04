proj11: proj11.client.o
	g++ proj11.client.o -o proj11.client

proj11.client.o: proj11.client.c
	g++ -Wall -c -g proj11.client.c 
	
clean:
	rm -f proj11.client.o
	rm -f proj11.client


