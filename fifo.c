#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "fifo.h"
#include "estado.h"

void inicializar_fifo() {
    mkfifo(FIFO_PATH, 0666);
}

void processar_mensagem_fifo(char *mensagem) {
    int id, linha, coluna;
    
    if (sscanf(mensagem, "%d %d %d", &id, &linha, &coluna) >= 2) { 
        if (id >= 0 && id < MAX_NAVIOS) {
            navios[id].coluna = coluna;
            navios[id].linha = linha;
        }
    }
}