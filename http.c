#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "http.h"
#include "estado.h"

// Função base para responder requisições com cabeçalho HTTP
void responder(int client, char *body) {
    char response[4096];
    sprintf(response,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Access-Control-Allow-Origin: *\r\n" // Essencial para o HTML conseguir ler os dados
        "Connection: close\r\n"
        "\r\n"
        "%s",
        body
    );
    write(client, response, strlen(response));
}

// Endpoint /status exigido pelo professor (mostra linhas ativas e contagem)
void endpoint_status(int client) {
    char json[1024];
    int p = 0, s = 0, f = 0;
    char linhas_json[512] = "";
    
    for(int i = 0; i < MAX_NAVIOS; i++) {
        if(navios[i].ativo) {
            char tmp[100];
            sprintf(tmp, "{\"linha\":%d, \"tipo\":\"%s\"},", navios[i].linha, navios[i].tipo);
            strcat(linhas_json, tmp);
            
            // Contabiliza os navios ativos
            if(strcmp(navios[i].tipo, "porta_avioes") == 0) p++;
            else if(strcmp(navios[i].tipo, "submarino") == 0) s++;
            else if(strcmp(navios[i].tipo, "fragata") == 0) f++;
        }
    }
    // Remove a última vírgula da lista JSON
    if(strlen(linhas_json) > 0) linhas_json[strlen(linhas_json)-1] = '\0';

    sprintf(json, "{ \"linhas\": [%s], \"quantidade\": {\"porta_avioes\":%d,\"submarinos\":%d,\"fragatas\":%d}}", linhas_json, p, s, f);
    responder(client, json);
}

// Endpoint /tiro?linha=X&coluna=Y
void endpoint_tiro(int client, int linha, int coluna) {
    if (ataques[linha][coluna]) {
        responder(client, "{\"resultado\":\"repetido\"}");
        return;
    }
    ataques[linha][coluna] = 1;

    char tipo[50];
    int pontos;
    int pid;

    if (verificar_acerto(linha, coluna, tipo, &pontos, &pid)) {
        kill(pid, SIGKILL); // Elimina o processo do navio atingido
        char json[200];
        sprintf(json, "{\"resultado\":\"acerto\",\"tipo\":\"%s\",\"pontos\":%d}", tipo, pontos);
        responder(client, json);
    } else {
        responder(client, "{\"resultado\":\"agua\"}");
    }
}

// Endpoint /estado para a página HTML desenhar o tabuleiro
void endpoint_estado(int client) {
    char json[2048] = "{ \"navios\": [";
    for(int i = 0; i < MAX_NAVIOS; i++) {
        char navio_json[256];
        sprintf(navio_json, "{\"id\":%d,\"tipo\":\"%s\",\"linha\":%d,\"coluna\":%d,\"ativo\":%d},",
                navios[i].id, navios[i].tipo, navios[i].linha, navios[i].coluna, navios[i].ativo);
        strcat(json, navio_json);
    }
    json[strlen(json)-1] = '\0';  
    strcat(json, "], \"acabou\": ");
    strcat(json, jogo_acabou() ? "true" : "false");
    strcat(json, " }");
    responder(client, json);
}

// Analisa a URL solicitada e direciona para a função correta
void processar_requisicao_http(int client, char *buffer) {
    char method[10], path[100];
    if (sscanf(buffer, "%s %s", method, path) < 2) return;

    if (strcmp(method, "GET") == 0) {
        if (strcmp(path, "/status") == 0) {
            endpoint_status(client);
        } else if (strcmp(path, "/estado") == 0) {
            endpoint_estado(client);
        } else if (strncmp(path, "/tiro?", 6) == 0) {
            int linha = -1, coluna = -1;
            sscanf(path, "/tiro?linha=%d&coluna=%d", &linha, &coluna);
            if (linha >= 0 && linha < TAB_LINHAS && coluna >= 0 && coluna < TAB_COLUNAS) {
                endpoint_tiro(client, linha, coluna);
            } else {
                responder(client, "{\"erro\":\"parametros invalidos\"}");
            }
        } else {
            responder(client, "{\"erro\":\"endpoint nao encontrado\"}");
        }
    } else {
        responder(client, "{\"erro\":\"metodo nao suportado\"}");
    }
}