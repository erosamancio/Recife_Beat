#include "raylib.h"
#include "musica_praieira.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// ==========================================
// CONFIGURAÇÕES GERAIS E ESTADOS
// ==========================================
typedef enum {
    MENU_PRINCIPAL,
    MENU_MUSICAS,
    MENU_AJUSTES,
    TRANSICAO,
    JOGANDO
} EstadoJogo;

int LARGURA_TELA;
int ALTURA_TELA;
int Y_ALVO;
#define RAIO_NOTA 30
#define VELOCIDADE 0.5f

int pontuacao = 0;
char mensagem_feedback[20] = "";
bool modoEditor = false;
const char* mapa_atual = "mapa_praieira.txt";

// ==========================================
// FUNÇÕES AUXILIARES
// ==========================================
int verificar_pontuacao(int y_bola) {
    int distancia = abs(y_bola - Y_ALVO);
    float sobreposicao = 1.0f - ((float)distancia / (RAIO_NOTA * 2));

    if (sobreposicao >= 0.85f) {
        TextCopy(mensagem_feedback, "PERFEITO!");
        return 300;
    }
    if (sobreposicao >= 0.50f) {
        TextCopy(mensagem_feedback, "MUITO BOM!");
        return 100;
    }
    if (sobreposicao >= 0.25f) {
        TextCopy(mensagem_feedback, "OK!");
        return 50;
    }
    return 0;
}

void resetar_notas() {
    Nota *atual = inicio;
    while (atual != NULL) {
        atual->ativa = true;
        atual = atual->prox;
    }
}

// ==========================================
// FUNÇÃO PRINCIPAL
// ==========================================
int main() {
    // Configura a janela para iniciar em Tela Cheia
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Recife Beat - A Praieira");

    // Captura a resolução real
    LARGURA_TELA = GetScreenWidth();
    ALTURA_TELA = GetScreenHeight();
    Y_ALVO = ALTURA_TELA - 150; 

    InitAudioDevice();

    // Carregamento de recursos gerais
    Texture2D mapaBase = LoadTexture("images/mapa_base.png");
    
    // ==========================================
    // CARREGAMENTO DA FONTE CUSTOMIZADA
    // ==========================================
    // Usamos LoadFontEx para carregar a fonte já em tamanho grande (60) e evitar desfoque
    Font fonteTitulo = LoadFontEx("fonts/PermanentMarker-Regular.ttf", 60, NULL, 0);

    // ==========================================
    // CARREGAMENTO DA ANIMAÇÃO DA CAPIVARA
    // ==========================================
    Texture2D framesCapivara[4];
    framesCapivara[0] = LoadTexture("images/capivara_base1.png");
    framesCapivara[1] = LoadTexture("images/capivara_base2.png");
    framesCapivara[2] = LoadTexture("images/capivara_base3.png");
    framesCapivara[3] = LoadTexture("images/capivara_base4.png");
    
    int frameAtualCapivara = 0;
    float timerAnimacaoCapivara = 0.0f;
    // ==========================================

    carregar_mapa(mapa_atual);
    Music musica = LoadMusicStream("audio/a_praieira.ogg");

    SetTargetFPS(60);

    EstadoJogo estadoAtual = MENU_PRINCIPAL;
    int opcaoMenu = 0;
    float volumeGeral = 1.0f;
    float alphaTransicao = 1.0f;
    bool deveFechar = false; 

    int deslocamentoX = (LARGURA_TELA - 600) / 2;

    int teclas[] = { KEY_C, KEY_V, KEY_N, KEY_M };

    while (!WindowShouldClose() && !deveFechar) {

        // ==========================================
        // LÓGICA DE ATUALIZAÇÃO (UPDATE)
        // ==========================================
        switch (estadoAtual) {
            case MENU_PRINCIPAL:
                if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
                    opcaoMenu = (opcaoMenu + 1) % 3;
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
                    opcaoMenu = (opcaoMenu - 1 + 3) % 3;
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    if (opcaoMenu == 0) { 
                        estadoAtual = MENU_MUSICAS; 
                        opcaoMenu = 0;
                    }
                    else if (opcaoMenu == 1) { 
                        estadoAtual = MENU_AJUSTES; 
                        opcaoMenu = 0;
                    }
                    else if (opcaoMenu == 2) { 
                        deveFechar = true; 
                    }
                }
                break;

            case MENU_MUSICAS:
                if (IsKeyPressed(KEY_ENTER)) {
                    estadoAtual = TRANSICAO;
                    PlayMusicStream(musica);
                }
                if (IsKeyPressed(KEY_ESCAPE)) estadoAtual = MENU_PRINCIPAL;
                break;

            case MENU_AJUSTES:
                if (IsKeyPressed(KEY_RIGHT) && volumeGeral < 1.0f) volumeGeral += 0.1f;
                if (IsKeyPressed(KEY_LEFT) && volumeGeral > 0.0f) volumeGeral -= 0.1f;
                SetMasterVolume(volumeGeral);
                if (IsKeyPressed(KEY_ESCAPE)) estadoAtual = MENU_PRINCIPAL;
                break;

            case TRANSICAO:
                UpdateMusicStream(musica);
                alphaTransicao -= 0.5f * GetFrameTime();
                if (alphaTransicao <= 0.0f) {
                    alphaTransicao = 0.0f;
                    estadoAtual = JOGANDO;
                }
                
                timerAnimacaoCapivara += GetFrameTime();
                if (timerAnimacaoCapivara >= 0.25f) { 
                    timerAnimacaoCapivara -= 0.25f;
                    frameAtualCapivara = (frameAtualCapivara + 1) % 4;
                }
                break;

            case JOGANDO:
                UpdateMusicStream(musica);

                timerAnimacaoCapivara += GetFrameTime();
                if (timerAnimacaoCapivara >= 0.25f) {
                    timerAnimacaoCapivara -= 0.25f;
                    frameAtualCapivara = (frameAtualCapivara + 1) % 4;
                }

                float tempo_atual_ms = GetMusicTimePlayed(musica) * 1000.0f;

                if (IsKeyPressed(KEY_ESCAPE)) {
                    estadoAtual = MENU_PRINCIPAL;
                    StopMusicStream(musica);
                    resetar_notas();
                    pontuacao = 0;
                    TextCopy(mensagem_feedback, "");
                    alphaTransicao = 1.0f;
                }

                if (((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_LEFT_SUPER)) &&
                    IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_X))) {
                    modoEditor = !modoEditor;
                    StopMusicStream(musica);
                    PlayMusicStream(musica);
                    resetar_notas();
                    TextCopy(mensagem_feedback, "");
                }

                if (modoEditor) {
                    for (int i = 0; i < 4; i++) {
                        if (IsKeyPressed(teclas[i])) {
                            inserir_nota((int)tempo_atual_ms, i);
                        }
                    }
                    if (IsKeyPressed(KEY_S)) {
                        salvar_mapa(mapa_atual);
                        TextCopy(mensagem_feedback, "MAPA SALVO!");
                    }
                    if (IsKeyPressed(KEY_DELETE)) {
                        liberar_notas();
                        TextCopy(mensagem_feedback, "MAPA LIMPO!");
                    }
                }
                else {
                    for (int i = 0; i < 4; i++) {
                        if (IsKeyPressed(teclas[i])) {
                            Nota *atual = inicio;
                            while (atual != NULL) {
                                if (atual->ativa && atual->botao == i) {
                                    int y_atual = Y_ALVO - (atual->tempo_ms - tempo_atual_ms) * VELOCIDADE;
                                    int pontos = verificar_pontuacao(y_atual);
                                    if (pontos > 0) {
                                        pontuacao += pontos;
                                        atual->ativa = false;
                                        break;
                                    }
                                }
                                atual = atual->prox;
                            }
                        }
                    }
                }
                break;
        }

        // ==========================================
        // LÓGICA DE DESENHO (DRAW)
        // ==========================================
        BeginDrawing();
        ClearBackground(BLACK);

        if (estadoAtual == MENU_PRINCIPAL) {
            // Desenhando o título com a fonte customizada
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "RECIFE BEAT", 60, 2);
            DrawTextEx(fonteTitulo, "RECIFE BEAT", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 4}, 60, 2, WHITE);

            // As opções do menu continuam com a fonte padrão (mas você pode mudar se quiser!)
            DrawText(opcaoMenu == 0 ? "> JOGAR <" : "JOGAR", LARGURA_TELA / 2 - 80, ALTURA_TELA / 2, 40, opcaoMenu == 0 ? YELLOW : GRAY);
            DrawText(opcaoMenu == 1 ? "> AJUSTES <" : "AJUSTES", LARGURA_TELA / 2 - 110, ALTURA_TELA / 2 + 70, 40, opcaoMenu == 1 ? YELLOW : GRAY);
            DrawText(opcaoMenu == 2 ? "> SAIR <" : "SAIR", LARGURA_TELA / 2 - 60, ALTURA_TELA / 2 + 140, 40, opcaoMenu == 2 ? YELLOW : GRAY);
        }
        else if (estadoAtual == MENU_MUSICAS) {
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "SELECIONE A MUSICA", 50, 2);
            DrawTextEx(fonteTitulo, "SELECIONE A MUSICA", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 4}, 50, 2, WHITE);

            DrawText("> A Praieira - Chico Science <", LARGURA_TELA / 2 - MeasureText("> A Praieira - Chico Science <", 40) / 2, ALTURA_TELA / 2, 40, YELLOW);
            DrawText("Pressione ESC para voltar", 30, ALTURA_TELA - 50, 20, GRAY);
        }
        else if (estadoAtual == MENU_AJUSTES) {
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "AJUSTES DE AUDIO", 50, 2);
            DrawTextEx(fonteTitulo, "AJUSTES DE AUDIO", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 4}, 50, 2, WHITE);

            DrawText(TextFormat("Volume Geral: < %0.0f%% >", volumeGeral * 100), LARGURA_TELA / 2 - 180, ALTURA_TELA / 2, 40, YELLOW);
            DrawText("Pressione ESC para voltar", 30, ALTURA_TELA - 50, 20, GRAY);
        }
        else if (estadoAtual == JOGANDO || estadoAtual == TRANSICAO) {
            
            Rectangle source = { 0.0f, 0.0f, (float)mapaBase.width, (float)mapaBase.height };
            Rectangle dest = { 0.0f, 0.0f, (float)LARGURA_TELA, (float)ALTURA_TELA };
            DrawTexturePro(mapaBase, source, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);

            // DESENHA A CAPIVARA ANIMADA
            float escalaCapivara = 0.3f; 
            float alturaCapivara = framesCapivara[frameAtualCapivara].height * escalaCapivara;
            float posCapivaraX = deslocamentoX + 650; 
            float posCapivaraY = (ALTURA_TELA / 2) - (alturaCapivara / 2);

            DrawTextureEx(
                framesCapivara[frameAtualCapivara], 
                (Vector2){ posCapivaraX, posCapivaraY }, 
                0.0f, 
                escalaCapivara, 
                WHITE
            );

            for (int i = 0; i < 4; i++) {
                DrawRectangle(deslocamentoX + i * 150, 0, 100, ALTURA_TELA, ColorAlpha(LIGHTGRAY, 0.6f));
                DrawCircleLines(deslocamentoX + 50 + i * 150, Y_ALVO, RAIO_NOTA + 2, BLACK);
                
                const char* teclasStr[] = {"C", "V", "N", "M"};
                DrawText(teclasStr[i], deslocamentoX + 40 + i * 150, Y_ALVO - 15, 30, DARKGRAY);
            }

            Nota *atual = inicio;
            float tempo_atual_ms = GetMusicTimePlayed(musica) * 1000.0f;

            while (atual != NULL) {
                if (atual->ativa) {
                    float y_pos = Y_ALVO - (atual->tempo_ms - tempo_atual_ms) * VELOCIDADE;

                    if (y_pos > ALTURA_TELA + 50) {
                        atual->ativa = false;
                    }

                    if (y_pos > -50 && y_pos < ALTURA_TELA) {
                        Color corNota = (tempo_atual_ms > 180000) ? ORANGE : MAROON;
                        DrawCircle(deslocamentoX + 50 + atual->botao * 150, (int)y_pos, RAIO_NOTA, corNota);
                    }
                }
                atual = atual->prox;
            }

            DrawText(TextFormat("PONTOS: %06d", pontuacao), 30, 30, 30, DARKGRAY);
            DrawText(mensagem_feedback, LARGURA_TELA / 2 - MeasureText(mensagem_feedback, 40) / 2, 80, 40, GOLD);
            
            // Título do jogo durante a gameplay com a fonte customizada
            Vector2 tamanhoGameplay = MeasureTextEx(fonteTitulo, "A PRAIEIRA - CHICO SCIENCE", 30, 2);
            DrawTextEx(fonteTitulo, "A PRAIEIRA - CHICO SCIENCE", (Vector2){LARGURA_TELA - tamanhoGameplay.x - 30, 30}, 30, 2, MAROON);

            if (modoEditor) {
                DrawRectangle(0, 0, LARGURA_TELA, 50, Fade(RED, 0.8f));
                DrawText("MODO EDITOR | TOQUE PARA GRAVAR | S: SALVAR | DELETE: LIMPAR", 30, 15, 20, WHITE);
            }

            if (estadoAtual == TRANSICAO) {
                DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, alphaTransicao));
            }
        }

        EndDrawing();
    }

    // ==========================================
    // LIBERAÇÃO DE RECURSOS
    // ==========================================
    for (int i = 0; i < 4; i++) {
        UnloadTexture(framesCapivara[i]);
    }
    UnloadTexture(mapaBase);
    UnloadFont(fonteTitulo); // Limpa a fonte da memória
    
    liberar_notas();
    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}