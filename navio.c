#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FIFO_PATH "/tmp/batalha_fifo"

void loop_navio(int id) {
    srand(time(NULL) + id);  // Seed diferente por navio
    int fd = open(FIFO_PATH, O_WRONLY);
    int linha = rand() % 10;
    int coluna = rand() % 10;
    char msg[100];

    while(1) {
        // Movimento randômico em linha e coluna
        linha = (linha + (rand() % 3 - 1) + 10) % 10;  // -1, 0, +1
        coluna = (coluna + (rand() % 3 - 1) + 10) % 10;
        sprintf(msg, "%d %d %d\n", id, linha, coluna);  // Incluir linha
        write(fd, msg, strlen(msg));
        sleep(5);
    }
}

int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Uso: ./navio <id>\n");
        return 1;
    }

    int id = atoi(argv[1]);

    loop_navio(id);

    return 0;
}