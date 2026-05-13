#include "ranking.h"

Score rankingTela[MAX_SCORES];
int qtdRankingTela = 0;

void trocar(Score* a, Score* b) {
    Score temp = *a;
    *a = *b;
    *b = temp;
}

int particao(Score arr[], int baixo, int alto){
    int pivo = arr[alto].pontos;
    int i = (baixo - 1);

    for(int j = baixo; j <= alto - 1; j++){
        if(arr[j].pontos >= pivo){
            i++;
            trocar(&arr[i], &arr[j]);
        }
    }
    trocar(&arr[i + 1], &arr[alto]);
    return (i + 1);
}

void quicksort(Score arr[], int baixo, int alto){
    if(baixo < alto){
        int pi = particao(arr, baixo, alto);
        quicksort(arr, baixo, pi - 1);
        quicksort(arr, pi + 1, alto);
    }
}

void salvar_score(const char* arquivo, const char* nome_jogador, int nova_pontuacao) {
    if (nova_pontuacao == 0) return;

    Score ranking[MAX_SCORES + 1];
    int qtd = 0;
    FILE *f = fopen(arquivo, "r");
    if(f != NULL){
        while(fscanf(f, "%s %d", ranking[qtd].nome, &ranking[qtd].pontos) != EOF){
            qtd++;
            if(qtd >= MAX_SCORES)break;
        }
        fclose(f);
    }

    TextCopy(ranking[qtd].nome, nome_jogador); 
    ranking[qtd].pontos = nova_pontuacao;
    qtd++;

    quicksort(ranking, 0, qtd - 1);

    f = fopen(arquivo, "w");
    if(f != NULL){
        int limite;
        if(qtd > MAX_SCORES){
            limite = MAX_SCORES;
        }else{ 
            limite = qtd;
        }
        for(int i = 0; i < limite; i++) {
            fprintf(f, "%s %d\n", ranking[i].nome, ranking[i].pontos);
        }
        fclose(f);
    }
}

void carregar_ranking_tela(const char* arquivo){
    qtdRankingTela = 0;
    FILE *f = fopen(arquivo, "r");
    if(f != NULL){
        while(fscanf(f, "%s %d", rankingTela[qtdRankingTela].nome, &rankingTela[qtdRankingTela].pontos) != EOF){
            qtdRankingTela++;
            if(qtdRankingTela >= MAX_SCORES) break;
        }
        fclose(f);
    }
}