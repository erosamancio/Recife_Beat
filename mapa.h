#ifndef MAPA_H
#define MAPA_H

#include <raylib.h>
#include <stdbool.h>

typedef struct nota {

    int tempo_ms;
    int botao;
    bool ativa;

    struct nota *prox;

} Nota;

extern Nota *inicio;

void inserir_nota(int tempo, int botao);

void liberar_notas();

void resetar_notas();

void salvar_mapa(const char *nomeArquivo);

void carregar_mapa(const char *nomeArquivo);

#endif