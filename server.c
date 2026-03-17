#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <fcntl.h>

#include "estado.h"
#include "fifo.h"
#include "http.h"

#define PORT 8080

void criar_navios() {
    for(int i = 0; i < MAX_NAVIOS; i++) {
        pid_t pid = fork();

        if(pid == 0) {
            // Processo filho substitui sua imagem pelo executável do navio
            char id_str[10];
            sprintf(id_str, "%d", i);
            execl("./navio", "navio", id_str, NULL);
            
            // Se o execl falhar, imprime o erro
            perror("Erro ao executar navio. Lembre-se de rodar o 'make all'");
            exit(1);
        }
        // Processo pai registra o PID
        navios[i].pid = pid;
    }
}

int main() {
    inicializar_navios();
    inicializar_fifo();
    criar_navios();

    int fifo = open(FIFO_PATH, O_RDONLY | O_NONBLOCK);
    int server = socket(AF_INET, SOCK_STREAM, 0);

    // Isso evita o erro "Address already in use" se você fechar e abrir rápido o servidor
    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 10);

    printf("Miniwebserver rodando na porta %d...\n", PORT);

    fd_set fds;

    while(1) {
        FD_ZERO(&fds);
        FD_SET(server, &fds);
        FD_SET(fifo, &fds);

        // O select precisa do maior descritor de arquivo + 1 para funcionar corretamente
        int max_fd = (server > fifo) ? server : fifo;
        
        select(max_fd + 1, &fds, NULL, NULL, NULL);

        // Verifica se a FIFO mandou dados (Navio se moveu)
        if(FD_ISSET(fifo, &fds)) {
            char buf[256];
            int n = read(fifo, buf, sizeof(buf) - 1);
            if(n > 0) {
                buf[n] = '\0';
                processar_mensagem_fifo(buf);
            }
        }

        // Verifica se chegou uma requisição web na porta 8080 (Ataque ou Atualização de página)
        if(FD_ISSET(server, &fds)) {
            int client = accept(server, NULL, NULL);
            char buffer[1024];
            int n = read(client, buffer, sizeof(buffer) - 1);
            if (n > 0) {
                buffer[n] = '\0';
                processar_requisicao_http(client, buffer);
            }
            close(client);
        }
    }
    return 0;
}