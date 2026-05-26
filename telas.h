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
    int comboErros;

    // Nossas Fontes
    Font fonteTitulo;
    Font fonteBebas; // Nova fonte para o Menu de Músicas

    // Texturas de fundo e elementos base
    Texture2D mapaBase;
    Texture2D framesCapivara[4];
    Texture2D framesMenuTitulo[6];
    
    // Texturas do Menu de Músicas
    Texture2D texMenuMusicaFundo;
    Texture2D texBotaoMusicaNormal;
    Texture2D texBotaoMusicaPlay;

    // Controle de Visibilidade
    bool exibindoRanking;

    Music musicaAtual;
    Music musicaMenu;
    
    int frameCapivara;
    float timerCapivara;
    int frameMenu;
    float timerMenu;

    Texture2D framesInimigoAtual[4]; 
    int frameInimigo;                
    float timerInimigo;

    const char* mapaAtualCaminho;
    const char* rankingAtualCaminho;

    Texture2D btnBase[4];      
    Texture2D btnClicado[4];   
    Texture2D btnNota[4];      
    float timerClickPista[4];
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