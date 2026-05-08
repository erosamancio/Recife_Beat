#ifndef MUSICA_PRAIEIRA_H
#define MUSICA_PRAIEIRA_H

#include <raylib.h>
#include <stdlib.h>

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

#endif