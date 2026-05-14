#ifndef TELAS_H
#define TELAS_H

#include "raylib.h"
#include "ranking.h"
#include "mapa.h"

typedef enum {
    MENU_PRINCIPAL,
    MENU_MUSICAS,
    MENU_AJUSTES,
    TRANSICAO,
    JOGANDO,
    INSERIR_NOME
} EstadoJogo;

typedef struct {
    int largura;
    int altura;
    int yAlvo;
    int deslocamentoX;
    
    EstadoJogo estadoAtual;
    int opcaoMenu;
    int opcaoMusica;
    float volumeGeral;
    float alphaTransicao;
    bool deveFechar;
    bool modoEditor;

    int pontuacao;
    char mensagemFeedback[20];
    char nomeInput[15];
    int contLetras;
    int teclas[4];

    Font fonteTitulo;
    Texture2D mapaBase;
    Texture2D framesCapivara[4];
    Texture2D framesMenuTitulo[6];
    Music musicaAtual;
    
    int frameCapivara;
    float timerCapivara;
    int frameMenu;
    float timerMenu;

    const char* mapaAtualCaminho;
    const char* rankingAtualCaminho;
} GameContext;

void InitGameContext(GameContext *ctx);
void UnloadGameResources(GameContext *ctx);

void UpdateMenuPrincipal(GameContext *ctx);
void UpdateMenuMusicas(GameContext *ctx);
void UpdateMenuAjustes(GameContext *ctx);
void UpdateTransicao(GameContext *ctx);
void UpdateJogando(GameContext *ctx);
void UpdateInserirNome(GameContext *ctx);

void DrawMenuPrincipal(GameContext *ctx);
void DrawMenuMusicas(GameContext *ctx);
void DrawMenuAjustes(GameContext *ctx);
void DrawTransicao(GameContext *ctx);
void DrawJogando(GameContext *ctx);
void DrawInserirNome(GameContext *ctx);

#endif