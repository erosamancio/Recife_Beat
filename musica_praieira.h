#ifndef MUSICA_PRAIEIRA_H
#define MUSICA_PRAIEIRA_H

#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct nota {
    int tempo_ms;
    int botao;
    bool ativa;

    struct nota *prox;
} Nota;

static Nota *inicio = NULL;

void inserir_nota(int tempo, int botao) {
    Nota *nova = malloc(sizeof(Nota));

    nova->tempo_ms = tempo;
    nova->botao = botao;
    nova->ativa = true;
    nova->prox = NULL;

    if (inicio == NULL) {
        inicio = nova;
        return;
    }

    Nota *atual = inicio;

    while (atual->prox != NULL) {
        atual = atual->prox;
    }

    atual->prox = nova;
}

void liberar_notas() {
    Nota *atual = inicio;

    while (atual != NULL) {
        Nota *temp = atual;
        atual = atual->prox;
        free(temp);
    }

    inicio = NULL;
}

void GerarFaseCompleta() {
    int tempo_base = 0;

    while (tempo_base < 216000) {

        if (tempo_base < 20000) {

            inserir_nota(tempo_base + 0, 1);
            inserir_nota(tempo_base + 3000, 2);

        }

        else if (tempo_base < 180000) {

            inserir_nota(tempo_base + 0, 1);
            inserir_nota(tempo_base + 576, 2);
            inserir_nota(tempo_base + 1728, 2);
            inserir_nota(tempo_base + 2448, 1);

            if (tempo_base % 12000 < 6000) {

                inserir_nota(tempo_base + 3600, 3);
                inserir_nota(tempo_base + 4896, 0);

            } else {

                inserir_nota(tempo_base + 4320, 0);
                inserir_nota(tempo_base + 4896, 0);
                inserir_nota(tempo_base + 4896, 1);

            }
        }

        else {

            inserir_nota(tempo_base + 0, 1);
            inserir_nota(tempo_base + 200, 2);
            inserir_nota(tempo_base + 400, 1);
            inserir_nota(tempo_base + 600, 2);
            inserir_nota(tempo_base + 800, 3);

        }

        tempo_base += 6336;
    }
}
void salvar_mapa(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) return;

    Nota *atual = inicio;
    while (atual != NULL) {
        fprintf(arquivo, "%d,%d\n", atual->tempo_ms, atual->botao);
        atual = atual->prox;
    }
    fclose(arquivo);
}

void carregar_mapa(const char *nomeArquivo) {
    liberar_notas(); // Limpa a lista atual antes de carregar uma nova
    
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        TraceLog(LOG_WARNING, "Arquivo de mapa nao encontrado. Iniciando vazio.");
        return;
    }

    int tempo, botao;
    // Lê o arquivo linha por linha (formato: tempo,botao)
    while (fscanf(arquivo, "%d,%d", &tempo, &botao) != EOF) {
        inserir_nota(tempo, botao);
    }

    fclose(arquivo);
    TraceLog(LOG_INFO, "Mapa %s carregado com sucesso!", nomeArquivo);
}

#endif