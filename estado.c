#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estado.h"

Navio navios[MAX_NAVIOS];
int ataques[TAB_LINHAS][TAB_COLUNAS];

void inicializar_navios() {
    int i, j;

    for(i=0; i<TAB_LINHAS; i++)
        for(j=0; j<TAB_COLUNAS; j++)
            ataques[i][j] = 0;

    navios[0] = (Navio){0, "porta_avioes", 0, 0, 5, 0, 1, 5, 0, 0};
    navios[1] = (Navio){1, "submarino", 2, 0, 3, 1, 1, 3, 0, 0};
    navios[2] = (Navio){2, "submarino", 2, 3, 3, 1, 1, 3, 0, 0};
    navios[3] = (Navio){3, "fragata", 5, 0, 2, 0, 1, 2, 0, 0};
    navios[4] = (Navio){4, "fragata", 7, 0, 2, 0, 1, 2, 0, 0};
    navios[5] = (Navio){5, "fragata", 9, 0, 2, 0, 1, 2, 0, 0};
}

int verificar_acerto(int linha, int coluna, char *tipo, int *pontos, int *pid) {
    for(int i=0; i<MAX_NAVIOS; i++) {
        if(navios[i].ativo) {
            int hit = 0;
            if(navios[i].orientacao == 0) {
                if(linha == navios[i].linha && coluna >= navios[i].coluna && coluna < navios[i].coluna + navios[i].tamanho) {
                    hit = 1;
                }
            } else {
                if(coluna == navios[i].coluna && linha >= navios[i].linha && linha < navios[i].linha + navios[i].tamanho) {
                    hit = 1;
                }
            }
            if(hit) {
                navios[i].celulas_atingidas++;
                if(navios[i].celulas_atingidas >= navios[i].tamanho) {
                    strcpy(tipo, navios[i].tipo);
                    *pontos = navios[i].pontos;
                    *pid = navios[i].pid;
                    navios[i].ativo = 0;
                    return 1;
                }
                return 0;
            }
        }
    }
    return 0;
}

int jogo_acabou() {
    for(int i=0; i<MAX_NAVIOS; i++) {
        if(navios[i].ativo) return 0;
    }
    return 1;
}