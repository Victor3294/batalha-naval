#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "fifo.h"
#include "estado.h" // Precisamos incluir para alterar a coluna/linha dos navios

void inicializar_fifo() {
    // Cria o named pipe (FIFO) com permissão de leitura e escrita
    mkfifo(FIFO_PATH, 0666);
}

void processar_mensagem_fifo(char *mensagem) {
    int id, linha, coluna;
    
    // O navio.c que você mandou envia 3 informações: ID, linha e coluna
    if (sscanf(mensagem, "%d %d %d", &id, &linha, &coluna) >= 2) { 
        if (id >= 0 && id < MAX_NAVIOS) {
            // Atualiza o estado do navio na memória compartilhada do miniwebserver
            navios[id].coluna = coluna;
            navios[id].linha = linha;
        }
    }
}