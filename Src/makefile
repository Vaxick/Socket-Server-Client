#Defines
executible=server.echo
file=server.echo.c
host=localhost
port=5000

#Connessione al server
connect:
	telnet $(host) $(port)

#Creo l'eseguibile
build:
	gcc -o $(executible) $(file)

#Rimuovo l'eseguibile
clean:
	rm -f $(executible)

#Avvio il server
run:
	./$(executible)