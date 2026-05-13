#ifndef RANKING_H
#define RANKING_H

#include "raylib.h"
#include <stdio.h>

#define MAX_SCORES 5

typedef struct {
    char nome[10];
    int pontos;
} Score;

extern Score rankingTela[MAX_SCORES];
extern int qtdRankingTela;

void trocar(Score* a, Score* b);
int particao(Score arr[], int baixo, int alto);
void quicksort(Score arr[], int baixo, int alto);
void salvar_score(const char* arquivo, const char* nome_jogador, int nova_pontuacao);
void carregar_ranking_tela(const char* arquivo);

#endif