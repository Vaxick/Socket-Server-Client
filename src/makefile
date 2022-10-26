#Eseguibili
server_executible=server.echo.exe
client_executible=client.echo.exe

#File.c
server_file=server.echo.c
client_file=client.echo.c

#Informazioni Server
host=127.0.0.1
port=5000

#Creo gli eseguibili
build:
	gcc -o $(server_executible) $(server_file)
	gcc -o $(client_executible) $(client_file)
#Rimuovo gli eseguibili
clean:
	rm $(server_executible) $(client_executible) 
#Avvio il server
start:
	./$(server_executible)
#Connessione al server
connect:
	./$(client_executible) $(host) $(port)

#Se la porta è occupata usare free per liberarla
free:
	sudo kill -9 `sudo lsof -t -i:$(port)`