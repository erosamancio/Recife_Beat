#include "telas.h"
#include <math.h>
#include <stdlib.h>

#define NUM_MUSICAS 5
#define RAIO_NOTA 30

const char* titulosMusicasRef[NUM_MUSICAS] = {
    "A Praieira - Chico Science", "Anunciacao - Alceu Valenca",
    "Frevo Mulher - Amelinha", "Leao do Norte - Lenine", "Voltei Recife - Alceu Valenca"
};

const char* audiosRef[NUM_MUSICAS] = {
    "audio/a_praieira.ogg", "audio/anunciacao.ogg", "audio/frevo_mulher.ogg",
    "audio/leao_do_norte.ogg", "audio/voltei_recife.ogg"
};

const char* mapasRef[NUM_MUSICAS] = {
    "mapas/mapa_praieira.txt", "mapas/mapa_anunciacao.txt", "mapas/mapa_frevo_mulher.txt",
    "mapas/mapa_leao_do_norte.txt", "mapas/mapa_voltei_recife.txt"
};

const char* rankingsRef[NUM_MUSICAS] = {
    "rankings/ranking_praieira.txt", "rankings/ranking_anunciacao.txt", "rankings/ranking_frevo_mulher.txt",
    "rankings/ranking_leao_do_norte.txt", "rankings/ranking_voltei_recife.txt"
};

static Texture2D texBom = {0};
static Texture2D texMuitoBom = {0};
static Texture2D texPerfeito = {0};
static bool texturasCarregadas = false;
static float feedbackTimer = 0.0f;
static int tipoFeedback = 0; 

void InitGameContext(GameContext *ctx) {
    ctx->largura = GetScreenWidth();
    ctx->altura = GetScreenHeight();
    ctx->yAlvo = ctx->altura - 150;
    ctx->deslocamentoX = (ctx->largura - 600) / 2;
    
    ctx->estadoAtual = MENU_PRINCIPAL;
    ctx->opcaoMenu = 0;
    ctx->opcaoMusica = 0;
    ctx->volumeGeral = 1.0f;
    ctx->alphaTransicao = 1.0f;
    ctx->deveFechar = false;
    ctx->modoEditor = false;
    ctx->pontuacao = 0;
    ctx->contLetras = 0;
    
    ctx->teclas[0] = KEY_C; ctx->teclas[1] = KEY_V; 
    ctx->teclas[2] = KEY_N; ctx->teclas[3] = KEY_M;

    ctx->frameCapivara = 0; ctx->timerCapivara = 0.0f;
    ctx->frameMenu = 0; ctx->timerMenu = 0.0f;
    
    ctx->musicaAtual = (Music){ 0 };

    if (!texturasCarregadas) {
        texBom = LoadTexture("images/mensagens/bom.png");
        texMuitoBom = LoadTexture("images/mensagens/muito_bom.png");
        texPerfeito = LoadTexture("images/mensagens/perfeito.png");
        texturasCarregadas = true;
    }
}

void UpdateMenuPrincipal(GameContext *ctx) {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) ctx->opcaoMenu = (ctx->opcaoMenu + 1) % 3;
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) ctx->opcaoMenu = (ctx->opcaoMenu - 1 + 3) % 3;

    if (IsKeyPressed(KEY_ENTER)) {
        if (ctx->opcaoMenu == 0) { 
            ctx->estadoAtual = MENU_MUSICAS;
            carregar_ranking_tela(rankingsRef[ctx->opcaoMusica]);
        }
        else if (ctx->opcaoMenu == 1) ctx->estadoAtual = MENU_AJUSTES;
        else if (ctx->opcaoMenu == 2) ctx->deveFechar = true;
    }

    ctx->timerMenu += GetFrameTime();
    if (ctx->timerMenu >= 0.25f) {
        ctx->timerMenu = 0.0f; 
        ctx->frameMenu = (ctx->frameMenu + 1) % 6;
    }
}

void UpdateMenuMusicas(GameContext *ctx) {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        ctx->opcaoMusica = (ctx->opcaoMusica + 1) % NUM_MUSICAS;
        carregar_ranking_tela(rankingsRef[ctx->opcaoMusica]);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        ctx->opcaoMusica = (ctx->opcaoMusica - 1 + NUM_MUSICAS) % NUM_MUSICAS;
        carregar_ranking_tela(rankingsRef[ctx->opcaoMusica]);
    }
    if (IsKeyPressed(KEY_ENTER)) {
        liberar_notas(); 
        
        if (IsMusicValid(ctx->musicaAtual)) {
            UnloadMusicStream(ctx->musicaAtual);
        }
        
        ctx->musicaAtual = LoadMusicStream(audiosRef[ctx->opcaoMusica]);
        ctx->musicaAtual.looping = false;
        
        ctx->mapaAtualCaminho = mapasRef[ctx->opcaoMusica]; 
        ctx->rankingAtualCaminho = rankingsRef[ctx->opcaoMusica];
        carregar_mapa(ctx->mapaAtualCaminho);
        ctx->estadoAtual = TRANSICAO;
        ctx->alphaTransicao = 1.0f;
        PlayMusicStream(ctx->musicaAtual);
    }
    if (IsKeyPressed(KEY_ESCAPE)) ctx->estadoAtual = MENU_PRINCIPAL;
}

void UpdateMenuAjustes(GameContext *ctx) {
    if (IsKeyPressed(KEY_RIGHT) && ctx->volumeGeral < 1.0f) ctx->volumeGeral += 0.1f;
    if (IsKeyPressed(KEY_LEFT) && ctx->volumeGeral > 0.0f) ctx->volumeGeral -= 0.1f;
    SetMasterVolume(ctx->volumeGeral);
    if (IsKeyPressed(KEY_ESCAPE)) ctx->estadoAtual = MENU_PRINCIPAL;
}

void UpdateTransicao(GameContext *ctx) {
    UpdateMusicStream(ctx->musicaAtual);
    ctx->alphaTransicao -= 0.5f * GetFrameTime();
    if (ctx->alphaTransicao <= 0.0f) ctx->estadoAtual = JOGANDO;
    
    ctx->timerCapivara += GetFrameTime();
    if (ctx->timerCapivara >= 0.25f) {
        ctx->timerCapivara = 0.0f;
        ctx->frameCapivara = (ctx->frameCapivara + 1) % 4;
    }
}

bool VerificaInputPista(GameContext *ctx, int pista){
    if (IsKeyPressed(ctx->teclas[pista])) return true;

    if(IsGamepadAvailable(0)){
        if (pista == 0 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) return true;   // Setinha Esquerda
        if (pista == 1 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) return true;  // Setinha Direita
        if (pista == 2 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) return true;  // Quadrado / X
        if (pista == 3 && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) return true; // Bola / B (Xbox)
    }

    return false;
}

void UpdateJogando(GameContext *ctx) {
    UpdateMusicStream(ctx->musicaAtual);
    float tempo_ms = GetMusicTimePlayed(ctx->musicaAtual) * 1000.0f;
    float duracao_total = GetMusicTimeLength(ctx->musicaAtual) * 1000.0f;

    if (feedbackTimer > 0.0f) {
        feedbackTimer -= GetFrameTime();
    }

    ctx->timerCapivara += GetFrameTime();
    if (ctx->timerCapivara >= 0.25f) {
        ctx->timerCapivara = 0.0f;
        ctx->frameCapivara = (ctx->frameCapivara + 1) % 4;
    }
    
    if (duracao_total > 0 && tempo_ms >= duracao_total - 50.0f) {
        StopMusicStream(ctx->musicaAtual);
        if (!ctx->modoEditor && ctx->pontuacao > 0) {
            ctx->estadoAtual = INSERIR_NOME;
            ctx->nomeInput[0] = '\0'; ctx->contLetras = 0;
        } else {
            ctx->estadoAtual = MENU_PRINCIPAL;
            ctx->pontuacao = 0;
        }
        resetar_notas();
    }

    if (IsKeyPressed(KEY_ESCAPE)) {
        StopMusicStream(ctx->musicaAtual);
        if (!ctx->modoEditor && ctx->pontuacao > 0) {
            ctx->estadoAtual = INSERIR_NOME;
            ctx->nomeInput[0] = '\0'; ctx->contLetras = 0;
        } else {
            ctx->estadoAtual = MENU_PRINCIPAL;
            ctx->pontuacao = 0;
        }
        resetar_notas();
    }

    if (((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER)) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_X))) {
        ctx->modoEditor = !ctx->modoEditor;
        StopMusicStream(ctx->musicaAtual);
        PlayMusicStream(ctx->musicaAtual);
        resetar_notas();
        TextCopy(ctx->mensagemFeedback, "");
        feedbackTimer = 0.0f;
    }

    if (ctx->modoEditor) {
        for (int i = 0; i < 4; i++){
            if(VerificaInputPista(ctx, i)) inserir_nota((int)tempo_ms, i);
        }
        if (IsKeyPressed(KEY_S)) {
            salvar_mapa(ctx->mapaAtualCaminho);
            TextCopy(ctx->mensagemFeedback, "MAPA SALVO!");
        }
        if (IsKeyPressed(KEY_DELETE)) {
            liberar_notas();
            TextCopy(ctx->mensagemFeedback, "MAPA LIMPO!");
        }
    } else {
        for(int i = 0; i < 4; i++){
            if(VerificaInputPista(ctx, i)){
                Nota *at = inicio;
                while(at != NULL){
                    if(at->ativa && at->botao == i){
                        
                        float vel_nota = 0.42f; 
                        if (duracao_total > 0) {
                            if (at->tempo_ms < duracao_total * 0.25f) vel_nota = 0.32f; 
                            else if (at->tempo_ms < duracao_total * 0.80f) vel_nota = 0.42f; 
                            else vel_nota = 0.55f; 
                        }
                        
                        float y = ctx->yAlvo - (at->tempo_ms - tempo_ms) * vel_nota;
                        float dist = fabsf(y - ctx->yAlvo);

                        if (dist < RAIO_NOTA * 2) {
                            float precisao = (1.0f - (dist / (RAIO_NOTA * 2.0f))) * 100.0f;
                            
                            if (precisao >= 80.0f) {
                                ctx->pontuacao += 100;
                                tipoFeedback = 3; 
                            } else if (precisao >= 30.0f) {
                                ctx->pontuacao += 75;
                                tipoFeedback = 2; 
                            } else {
                                ctx->pontuacao += 50;
                                tipoFeedback = 1; 
                            }
                            
                            feedbackTimer = 0.5f; 
                            at->ativa = false;
                            break;
                        }
                    }
                    at = at->prox;
                }
            }
        }
    }
}

void UpdateInserirNome(GameContext *ctx) {
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (ctx->contLetras < 15)) {
            ctx->nomeInput[ctx->contLetras] = (char)key;
            ctx->nomeInput[ctx->contLetras + 1] = '\0';
            ctx->contLetras++;
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (ctx->contLetras > 0) {
            ctx->contLetras--;
            ctx->nomeInput[ctx->contLetras] = '\0';
        }
    }
    if (IsKeyPressed(KEY_ENTER) && ctx->contLetras > 0) {
        for (int i = 0; i < ctx->contLetras; i++) {
            if (ctx->nomeInput[i] == ' ') ctx->nomeInput[i] = '_';
        }
        salvar_score(ctx->rankingAtualCaminho, ctx->nomeInput, ctx->pontuacao);
        ctx->estadoAtual = MENU_PRINCIPAL;
        ctx->pontuacao = 0;
        TextCopy(ctx->mensagemFeedback, "");
    }
}

void DrawMenuPrincipal(GameContext *ctx) {
    Rectangle src = {0, 0, (float)ctx->framesMenuTitulo[ctx->frameMenu].width, (float)ctx->framesMenuTitulo[ctx->frameMenu].height};
    Rectangle dst = {0, 0, (float)ctx->largura, (float)ctx->altura};
    DrawTexturePro(ctx->framesMenuTitulo[ctx->frameMenu], src, dst, (Vector2){0,0}, 0, WHITE);

    int posX = ctx->largura - 400;
    const char* opts[] = {"JOGAR", "AJUSTES", "SAIR"};
    
    for(int i = 0; i < 3; i++) {
        float y = (ctx->altura / 2) + (i * 70);
        float size = 40.0f;
        
        if(i == ctx->opcaoMenu) {
            Vector2 tSize = MeasureTextEx(ctx->fonteTitulo, opts[i], size, 2);
            DrawRectangleRounded((Rectangle){(float)posX - 20, y - 5, tSize.x + 40, size + 10}, 0.5f, 5, ORANGE);
            DrawTextEx(ctx->fonteTitulo, opts[i], (Vector2){(float)posX, y}, size, 2, BLACK);
        } else {
            DrawTextEx(ctx->fonteTitulo, opts[i], (Vector2){(float)posX, y}, size, 2, GRAY);
        }
    }
}

void DrawMenuMusicas(GameContext *ctx) {
    Vector2 tamTitulo = MeasureTextEx(ctx->fonteTitulo, "SELECIONE A MUSICA", 50, 2);
    DrawTextEx(ctx->fonteTitulo, "SELECIONE A MUSICA", (Vector2){ctx->largura / 2 - tamTitulo.x / 2, ctx->altura / 6}, 50, 2, WHITE);

    for(int i = 0; i < NUM_MUSICAS; i++){
        Color corTexto = (i == ctx->opcaoMusica) ? YELLOW : GRAY;
        const char* textoMusica = (i == ctx->opcaoMusica) ? TextFormat("> %s <", titulosMusicasRef[i]) : titulosMusicasRef[i];
        
        float larguraTexto = MeasureTextEx(ctx->fonteTitulo, textoMusica, 40, 2).x;
        DrawTextEx(ctx->fonteTitulo, textoMusica, (Vector2){(ctx->largura / 2) - larguraTexto / 2 - 200, ctx->altura / 3 + (i * 60)}, 40, 2, corTexto);
    }
    
    float posXRanking = ctx->largura - 450; 
    float posYRanking = ctx->altura / 3;
    
    DrawTextEx(ctx->fonteTitulo, "TOP 5 - RANKING", (Vector2){posXRanking, posYRanking - 50}, 30, 2, GOLD);

    if(qtdRankingTela == 0){
        DrawTextEx(ctx->fonteTitulo, "Sem pontuacoes ainda...", (Vector2){posXRanking, posYRanking}, 20, 2, LIGHTGRAY);
        DrawTextEx(ctx->fonteTitulo, "Jogue e seja o primeiro!", (Vector2){posXRanking, posYRanking + 30}, 20, 2, DARKGRAY);
    }else{
        for(int i = 0; i < qtdRankingTela; i++){
            const char* linhaRank = TextFormat("%d. %s - %06d", i + 1, rankingTela[i].nome, rankingTela[i].pontos);
            Color corRank = WHITE;
            if (i == 0) corRank = GOLD;
            else if (i == 1) corRank = LIGHTGRAY;
            else if (i == 2) corRank = ORANGE;

            DrawTextEx(ctx->fonteTitulo, linhaRank, (Vector2){posXRanking, posYRanking + (i * 35)}, 25, 2, corRank);
        }
    }

    DrawTextEx(ctx->fonteTitulo, "Pressione ESC para voltar", (Vector2){30, ctx->altura - 50}, 20, 2, GRAY);
}

void DrawMenuAjustes(GameContext *ctx) {
    Vector2 tamTitulo = MeasureTextEx(ctx->fonteTitulo, "AJUSTES DE AUDIO", 50, 2);
    DrawTextEx(ctx->fonteTitulo, "AJUSTES DE AUDIO", (Vector2){ctx->largura / 2 - tamTitulo.x / 2, ctx->altura / 4}, 50, 2, WHITE);

    DrawTextEx(ctx->fonteTitulo, TextFormat("Volume Geral: < %0.0f%% >", ctx->volumeGeral * 100), (Vector2){ctx->largura / 2 - 180, ctx->altura / 2}, 40, 2, YELLOW);
    
    DrawTextEx(ctx->fonteTitulo, "Pressione ESC para voltar", (Vector2){30, ctx->altura - 50}, 20, 2, GRAY);
}

void DrawJogando(GameContext *ctx) {
    DrawTexturePro(ctx->mapaBase, (Rectangle){0,0,ctx->mapaBase.width, ctx->mapaBase.height},
        (Rectangle){0,0,ctx->largura, ctx->altura}, (Vector2){0,0}, 0, WHITE);

    float escala = 0.3f;
    float hCap = ctx->framesCapivara[ctx->frameCapivara].height * escala;
    DrawTextureEx(ctx->framesCapivara[ctx->frameCapivara], 
        (Vector2){ctx->deslocamentoX + 550, ctx->altura/2 - (hCap/2)}, 0, escala, WHITE);

    float y_horizonte = ctx->altura / 2.2f;
    float centro_pistas = ctx->deslocamentoX + 300.0f;
    float progresso_tela_toda = (ctx->altura - y_horizonte) / (ctx->yAlvo - y_horizonte);
    float escala_tela_toda = 0.2f + (0.8f * progresso_tela_toda);

    Color corFundoPista = ColorAlpha(BLACK, 0.6f);

    for(int i = 0; i < 4; i++){
        float x_base_esq = ctx->deslocamentoX + i * 150;
        float x_topo_esq = centro_pistas + (x_base_esq - centro_pistas) * 0.2f;
        float x_fim_esq = centro_pistas + (x_base_esq - centro_pistas) * escala_tela_toda;

        float x_base_dir = ctx->deslocamentoX + (i + 1) * 150;
        float x_topo_dir = centro_pistas + (x_base_dir - centro_pistas) * 0.2f;
        float x_fim_dir = centro_pistas + (x_base_dir - centro_pistas) * escala_tela_toda;

        Vector2 topoEsq = {x_topo_esq, y_horizonte};
        Vector2 baseEsq = {x_fim_esq, ctx->altura};
        Vector2 baseDir = {x_fim_dir, ctx->altura};
        Vector2 topoDir = {x_topo_dir, y_horizonte};

        DrawTriangle(topoEsq, baseEsq, baseDir, corFundoPista);
        DrawTriangle(topoEsq, baseDir, topoDir, corFundoPista);
    }

    for(int i = 0; i <= 4; i++){
        float x_base_linha = ctx->deslocamentoX + i * 150;
        float x_topo_linha = centro_pistas + (x_base_linha - centro_pistas) * 0.2f; 
        float x_fim_linha = centro_pistas + (x_base_linha - centro_pistas) * escala_tela_toda;
        
        DrawLineEx((Vector2){x_topo_linha, y_horizonte}, (Vector2){x_fim_linha, ctx->altura}, 4.0f, ColorAlpha(LIGHTGRAY, 0.5f));
    }

    for(int i = 0; i < 4; i++){
        float x_base_pista = ctx->deslocamentoX + 75 + i * 150;
        DrawCircleLines(x_base_pista, ctx->yAlvo, RAIO_NOTA + 2, BLACK);
        
        const char* teclasStr[] = {"C", "V", "N", "M"};
        DrawTextEx(ctx->fonteTitulo, teclasStr[i], (Vector2){x_base_pista - 10, ctx->yAlvo - 15}, 30, 2, WHITE);
    }

    float tempo = GetMusicTimePlayed(ctx->musicaAtual) * 1000.0f;
    float duracao_total = GetMusicTimeLength(ctx->musicaAtual) * 1000.0f;
    
    Nota *at = inicio;
    while(at != NULL) {
        if(at->ativa) {
            float vel_nota = 0.42f; 
            if (duracao_total > 0) {
                if (at->tempo_ms < duracao_total * 0.25f) vel_nota = 0.32f; 
                else if (at->tempo_ms < duracao_total * 0.80f) vel_nota = 0.42f; 
                else vel_nota = 0.55f; 
            }
            
            float y = ctx->yAlvo - (at->tempo_ms - tempo) * vel_nota;
            
            if(y > ctx->altura + 50){
                at->ativa = false;
            }

            if(y > y_horizonte && y < ctx->altura){
                float progresso = (y - y_horizonte) / (ctx->yAlvo - y_horizonte);
                if(progresso < 0.0f) progresso = 0.0f;
                float escala_perspectiva = 0.2f + (0.8f * progresso);
                float x_base_nota = ctx->deslocamentoX + 75 + at->botao * 150;
                
                float x_atual = centro_pistas + (x_base_nota - centro_pistas) * escala_perspectiva;
                float raio_atual = RAIO_NOTA * escala_perspectiva;

                Color corNota = (tempo > 180000) ? ORANGE : MAROON;
                DrawCircle((int)x_atual, (int)y, raio_atual, corNota);
            }
        }
        at = at->prox;
    }

    DrawTextEx(ctx->fonteTitulo, TextFormat("PONTOS: %06d", ctx->pontuacao), (Vector2){30, 30}, 30, 2, WHITE);
    
    if (ctx->modoEditor) {
        float msgLargura = MeasureTextEx(ctx->fonteTitulo, ctx->mensagemFeedback, 40, 2).x;
        DrawTextEx(ctx->fonteTitulo, ctx->mensagemFeedback, (Vector2){ctx->largura / 2 - msgLargura / 2, 80}, 40, 2, GOLD);
    } else {
        if (feedbackTimer > 0.0f) {
            Texture2D texDesenhar = texBom;
            if (tipoFeedback == 2) texDesenhar = texMuitoBom;
            else if (tipoFeedback == 3) texDesenhar = texPerfeito;
            
            float escalaTex = 380.0f / (float)texDesenhar.height;
            float drawWidth = texDesenhar.width * escalaTex;
            
            Vector2 pos = { (float)ctx->largura / 2.0f - drawWidth / 2.0f, 80.0f };
            DrawTextureEx(texDesenhar, pos, 0.0f, escalaTex, WHITE);
        }
    }
    
    Vector2 tamGameplay = MeasureTextEx(ctx->fonteTitulo, titulosMusicasRef[ctx->opcaoMusica], 30, 2);
    DrawTextEx(ctx->fonteTitulo, titulosMusicasRef[ctx->opcaoMusica], (Vector2){ctx->largura - tamGameplay.x - 30, 30}, 30, 2, WHITE);

    if(ctx->modoEditor){
        DrawRectangle(0, 0, ctx->largura, 50, Fade(RED, 0.8f));
        DrawTextEx(ctx->fonteTitulo, "MODO EDITOR | TOQUE PARA GRAVAR | S: SALVAR | DELETE: LIMPAR", (Vector2){30, 15}, 20, 2, WHITE);
    }
}

void DrawTransicao(GameContext *ctx) {
    DrawJogando(ctx);
    DrawRectangle(0, 0, ctx->largura, ctx->altura, ColorAlpha(BLACK, ctx->alphaTransicao));
}

void DrawInserirNome(GameContext *ctx) {
    Vector2 tamTitulo = MeasureTextEx(ctx->fonteTitulo, "FIM DE JOGO", 60, 2);
    DrawTextEx(ctx->fonteTitulo, "FIM DE JOGO", (Vector2){ctx->largura / 2 - tamTitulo.x / 2, ctx->altura / 4}, 60, 2, WHITE);

    const char* ptsStr = TextFormat("PONTUACAO: %d", ctx->pontuacao);
    float ptsLargura = MeasureTextEx(ctx->fonteTitulo, ptsStr, 40, 2).x;
    DrawTextEx(ctx->fonteTitulo, ptsStr, (Vector2){ctx->largura / 2 - ptsLargura / 2, ctx->altura / 2 - 60}, 40, 2, GOLD);
    
    float digLargura = MeasureTextEx(ctx->fonteTitulo, "DIGITE SEU NOME:", 30, 2).x;
    DrawTextEx(ctx->fonteTitulo, "DIGITE SEU NOME:", (Vector2){ctx->largura / 2 - digLargura / 2, ctx->altura / 2 + 20}, 30, 2, GRAY);
    
    DrawRectangle(ctx->largura / 2 - 200, ctx->altura / 2 + 70, 400, 50, LIGHTGRAY);
    DrawRectangleLines(ctx->largura / 2 - 200, ctx->altura / 2 + 70, 400, 50, DARKGRAY);
    
    float nomeLargura = MeasureTextEx(ctx->fonteTitulo, ctx->nomeInput, 30, 2).x;
    DrawTextEx(ctx->fonteTitulo, ctx->nomeInput, (Vector2){ctx->largura / 2 - nomeLargura / 2, ctx->altura / 2 + 80}, 30, 2, BLACK);

    if (((int)(GetTime() * 2)) % 2 == 0 && ctx->contLetras < 15) {
        DrawTextEx(ctx->fonteTitulo, "_", (Vector2){ctx->largura / 2 + nomeLargura / 2 + 5, ctx->altura / 2 + 80}, 30, 2, MAROON);
    }

    float enterLargura = MeasureTextEx(ctx->fonteTitulo, "Pressione ENTER para salvar", 20, 2).x;
    DrawTextEx(ctx->fonteTitulo, "Pressione ENTER para salvar", (Vector2){ctx->largura / 2 - enterLargura / 2, ctx->altura - 100}, 20, 2, DARKGRAY);
}

void UnloadGameResources(GameContext *ctx) {
    for(int i=0; i<4; i++) UnloadTexture(ctx->framesCapivara[i]);
    for(int i=0; i<6; i++) UnloadTexture(ctx->framesMenuTitulo[i]);
    UnloadTexture(ctx->mapaBase);
    UnloadFont(ctx->fonteTitulo);
    if (IsMusicValid(ctx->musicaAtual)) {
        UnloadMusicStream(ctx->musicaAtual);
    }
    
    if (texturasCarregadas) {
        UnloadTexture(texBom);
        UnloadTexture(texMuitoBom);
        UnloadTexture(texPerfeito);
        texturasCarregadas = false;
    }
}