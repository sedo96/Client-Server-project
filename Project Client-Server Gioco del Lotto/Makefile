CFLAGS = -Wall
CC = gcc

serverobj: Server/*.c
	gcc -Wall -c $^ 
	mv *.o Build/Server
	
messageobj: Messages/*.c
	gcc -Wall -c $^ 
	mv *.o Build/Messages

Server/lotto_server: serverobj messageobj
	gcc Build/Server/*.o Build/Messages/*.o -Wall -lpthread -o $@ 
	

clientobj: Client/*.c
	gcc -Wall -c $^ 
	mv *.o Build/Client


Client/lotto_client: clientobj messageobj
	gcc Build/Client/*.o Build/Messages/*.o -Wall -lpthread -o $@ 

	 
clean:
	rm Build/Server/*.o
	rm Build/Client/*.o
	rm Build/Messages/*.o
	rm Server/lotto_server
	rm Client/lotto_client
