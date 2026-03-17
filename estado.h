#ifndef ESTADO_H
#define ESTADO_H

#include <sys/types.h>  // Adicionado para definir pid_t

#define MAX_NAVIOS 6
#define TAB_LINHAS 10
#define TAB_COLUNAS 10

typedef struct {
    int id;
    char tipo[20];
    int linha;
    int coluna;
    int tamanho;  // Novo: tamanho do navio
    int orientacao;  // Novo: 0=horizontal, 1=vertical
    int ativo;
    int pontos;
    pid_t pid;
    int celulas_atingidas;  // Novo: contador de células atingidas
} Navio;

typedef struct {
    int linha;
    int coluna;
    char ip[64];
} Ataque;

extern Navio navios[MAX_NAVIOS];
extern int ataques[TAB_LINHAS][TAB_COLUNAS];

void inicializar_navios();
int verificar_acerto(int linha, int coluna, char *tipo, int *pontos, int *pid);
int jogo_acabou();  // Novo: verifica se todos navios afundaram

#endif