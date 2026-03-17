#ifndef FIFO_H
#define FIFO_H

#define FIFO_PATH "/tmp/batalha_fifo"

void inicializar_fifo();
void processar_mensagem_fifo(char *mensagem);

#endif