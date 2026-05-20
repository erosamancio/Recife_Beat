#include "mapa.h"

Nota *inicio = NULL;
Nota *fim = NULL; 

void inserir_nota(int tempo, int botao) {
    Nota *nova = malloc(sizeof(Nota));

    nova->tempo_ms = tempo;
    nova->botao = botao;
    nova->ativa = true;
    nova->prox = NULL;


    if (inicio == NULL) {
        inicio = nova;
        fim = nova;
        return;
    }


    fim->prox = nova;
    fim = nova;
}

void liberar_notas() {
    Nota *atual = inicio;

    while (atual != NULL) {
        Nota *temp = atual;
        atual = atual->prox;
        free(temp);
    }

    inicio = NULL;
    fim = NULL;
}

void resetar_notas() {
    Nota *atual = inicio;
    while (atual != NULL) {
        atual->ativa = true;
        atual = atual->prox;
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
    liberar_notas(); 
    
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        TraceLog(LOG_WARNING, "Arquivo de mapa nao encontrado. Iniciando vazio.");
        return;
    }

    int tempo, botao;
    while (fscanf(arquivo, "%d,%d", &tempo, &botao) != EOF) {
        inserir_nota(tempo, botao);
    }

    fclose(arquivo);
    TraceLog(LOG_INFO, "Mapa %s carregado com sucesso!", nomeArquivo);
}