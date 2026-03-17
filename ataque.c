#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main() {
    // 1. Ler o IP do arquivo
    FILE *f_ips = fopen("ips.txt", "r");
    if (!f_ips) { 
        printf("Erro: Arquivo ips.txt não encontrado!\n"); 
        return 1; 
    }
    char ip_alvo[64];
    fscanf(f_ips, "%s", ip_alvo);
    fclose(f_ips);

    srand(time(NULL));
    int pontos_totais = 0;
    int navios_afundados = 0;
    int tiros_usados = 0;

    printf("Iniciando ataque contra o IP: %s\n", ip_alvo);

    // 2. Loop de até 100 tiros
    for(int i = 0; i < 100; i++) {
        int linha = rand() % 10;
        int coluna = rand() % 10;
        
        char cmd[256];
        char resposta[512];

        // curl no modo silencioso (-s) para não sujar a tela com logs de download
        sprintf(cmd, "curl -s \"http://%s:8080/tiro?linha=%d&coluna=%d\"", ip_alvo, linha, coluna);

        // Executa o curl e abre um pipe (popen) para ler a saída
        FILE *fp = popen(cmd, "r");
        if (fp == NULL) {
            printf("Falha ao executar o curl.\n");
            continue;
        }

        tiros_usados++;

        // Lê a resposta JSON enviada pelo servidor atacado
        if (fgets(resposta, sizeof(resposta), fp) != NULL) {
            // Verifica se a string contém "acerto"
            if (strstr(resposta, "\"acerto\"")) {
                printf("[Tiro %03d] ACERTO! Alvo: (%d,%d) | Resposta: %s\n", tiros_usados, linha, coluna, resposta);
                navios_afundados++; // Pela sua lógica do servidor, 1 acerto já afunda o navio atual.
                
                // Contabiliza os pontos baseados na resposta
                if (strstr(resposta, "porta_avioes")) pontos_totais += 5;
                else if (strstr(resposta, "submarino")) pontos_totais += 3;
                else if (strstr(resposta, "fragata")) pontos_totais += 2;
            }
        }
        pclose(fp);
    }

    // 3. Relatório final (Requisito 8 do projeto)
    printf("\n==================================\n");
    printf("        RELATÓRIO DE ATAQUE         \n");
    printf("==================================\n");
    printf("IP Alvo...........: %s\n", ip_alvo);
    printf("Tiros disparados..: %d/100\n", tiros_usados);
    printf("Navios afundados..: %d\n", navios_afundados);
    printf("Pontuação Total...: %d pontos\n", pontos_totais);
    printf("==================================\n");

    return 0;
}