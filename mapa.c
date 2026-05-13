#include "mapa.h"

#include <stdlib.h>
#include <stdio.h>

// =========================
// LISTA GLOBAL
// =========================
Nota *inicio = NULL;

// =========================
// INSERIR NOTA
// =========================
void inserir_nota(int tempo, int botao) {

    Nota *nova = malloc(sizeof(Nota));

    if (nova == NULL)
        return;

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

// =========================
// LIBERAR LISTA
// =========================
void liberar_notas() {

    Nota *atual = inicio;

    while (atual != NULL) {

        Nota *temp = atual;

        atual = atual->prox;

        free(temp);
    }

    inicio = NULL;
}

// =========================
// RESETAR NOTAS
// =========================
void resetar_notas() {

    Nota *atual = inicio;

    while (atual != NULL) {

        atual->ativa = true;

        atual = atual->prox;
    }
}

// =========================
// SALVAR MAPA
// =========================
void salvar_mapa(const char *nomeArquivo) {

    FILE *arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL)
        return;

    Nota *atual = inicio;

    while (atual != NULL) {

        fprintf(
            arquivo,
            "%d,%d\n",
            atual->tempo_ms,
            atual->botao
        );

        atual = atual->prox;
    }

    fclose(arquivo);
}

// =========================
// CARREGAR MAPA
// =========================
void carregar_mapa(const char *nomeArquivo) {

    liberar_notas();

    FILE *arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {

        TraceLog(
            LOG_WARNING,
            "Mapa nao encontrado."
        );

        return;
    }

    int tempo;
    int botao;

    while (
        fscanf(
            arquivo,
            "%d,%d",
            &tempo,
            &botao
        ) != EOF
    ) {

        inserir_nota(
            tempo,
            botao
        );
    }

    fclose(arquivo);

    TraceLog(
        LOG_INFO,
        "Mapa carregado com sucesso!"
    );
}