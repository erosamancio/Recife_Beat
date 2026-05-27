#include "mapa.h"

Nota *inicio = NULL;
Nota *fim = NULL; 

// FUNÇÃO 1 DA ESTRUTURA DE DADOS
// Aloca memória dinamicamente e insere um novo nó no final da lista
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

// FUNÇÃO 2 DA ESTRUTURA DE DADOS
// Percorre a lista encadeada e libera a memória de cada nó
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

// FUNÇÃO 3 DA ESTRUTURA DE DADOS
// Percorre a lista encadeada reativando os nós para o jogador poder reiniciar a música
void resetar_notas() {
    Nota *atual = inicio;
    while (atual != NULL) {
        atual->ativa = true;
        atual = atual->prox;
    }
}

// FUNÇÃO 4 DA ESTRUTURA DE DADOS
// Percorre a lista encadeada acessando os dados de cada nó e gravando no arquivo .txt
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

// FUNÇÃO 5 DA ESTRUTURA DE DADOS
// Lê o arquivo .txt e popula a lista encadeada na memória em tempo de execução
void carregar_mapa(const char *nomeArquivo) {
    liberar_notas(); // Limpa qualquer mapa que já esteja na memória antes de carregar um novo
    
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        TraceLog(LOG_WARNING, "Arquivo de mapa nao encontrado. Iniciando vazio.");
        return;
    }

    int tempo, botao;
    // Enquanto houver linhas no arquivo, cria um novo nó na lista encadeada
    while (fscanf(arquivo, "%d,%d", &tempo, &botao) != EOF) {
        inserir_nota(tempo, botao);
    }

    fclose(arquivo);
    TraceLog(LOG_INFO, "Mapa %s carregado com sucesso!", nomeArquivo);
}