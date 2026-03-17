all:
	gcc server.c estado.c http.c fifo.c -o server
	gcc navio.c -o navio
	gcc ataque.c -o ataque