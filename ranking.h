#ifndef RANKING_H
#define RANKING_H

#define MAX_SCORES 5

typedef struct {

    char nome[10];
    int pontos;

} Score;

extern Score rankingTela[MAX_SCORES];

extern int qtdRankingTela;

void salvar_score(
    const char* arquivo,
    const char* nome_jogador,
    int nova_pontuacao
);

void carregar_ranking_tela(
    const char* arquivo
);

#endif