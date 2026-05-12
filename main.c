#include "raylib.h"
#include "musica_praieira.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>


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

int main() {

    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(0, 0, "Recife Beat - A Praieira");


    LARGURA_TELA = GetScreenWidth();
    ALTURA_TELA = GetScreenHeight();
    Y_ALVO = ALTURA_TELA - 150; 

    InitAudioDevice();


    Texture2D mapaBase = LoadTexture("images/mapa_base.png");
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
                break;

            case JOGANDO:
                UpdateMusicStream(musica);

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


        BeginDrawing();
        ClearBackground(BLACK);

        if (estadoAtual == MENU_PRINCIPAL) {
            DrawText("RECIFE BEAT", LARGURA_TELA / 2 - MeasureText("RECIFE BEAT", 60) / 2, ALTURA_TELA / 4, 60, WHITE);
            DrawText(opcaoMenu == 0 ? "> JOGAR <" : "JOGAR", LARGURA_TELA / 2 - 80, ALTURA_TELA / 2, 40, opcaoMenu == 0 ? YELLOW : GRAY);
            DrawText(opcaoMenu == 1 ? "> AJUSTES <" : "AJUSTES", LARGURA_TELA / 2 - 110, ALTURA_TELA / 2 + 70, 40, opcaoMenu == 1 ? YELLOW : GRAY);
            DrawText(opcaoMenu == 2 ? "> SAIR <" : "SAIR", LARGURA_TELA / 2 - 60, ALTURA_TELA / 2 + 140, 40, opcaoMenu == 2 ? YELLOW : GRAY);
        }
        else if (estadoAtual == MENU_MUSICAS) {
            DrawText("SELECIONE A MUSICA", LARGURA_TELA / 2 - MeasureText("SELECIONE A MUSICA", 50) / 2, ALTURA_TELA / 4, 50, WHITE);
            DrawText("> A Praieira - Chico Science <", LARGURA_TELA / 2 - MeasureText("> A Praieira - Chico Science <", 40) / 2, ALTURA_TELA / 2, 40, YELLOW);
            DrawText("Pressione ESC para voltar", 30, ALTURA_TELA - 50, 20, GRAY);
        }
        else if (estadoAtual == MENU_AJUSTES) {
            DrawText("AJUSTES DE AUDIO", LARGURA_TELA / 2 - MeasureText("AJUSTES DE AUDIO", 50) / 2, ALTURA_TELA / 4, 50, WHITE);
            DrawText(TextFormat("Volume Geral: < %0.0f%% >", volumeGeral * 100), LARGURA_TELA / 2 - 180, ALTURA_TELA / 2, 40, YELLOW);
            DrawText("Pressione ESC para voltar", 30, ALTURA_TELA - 50, 20, GRAY);
        }
        else if (estadoAtual == JOGANDO || estadoAtual == TRANSICAO) {
            

            Rectangle source = { 0.0f, 0.0f, (float)mapaBase.width, (float)mapaBase.height };
            Rectangle dest = { 0.0f, 0.0f, (float)LARGURA_TELA, (float)ALTURA_TELA };
            DrawTexturePro(mapaBase, source, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);


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
            DrawText("A PRAIEIRA - CHICO SCIENCE", LARGURA_TELA - MeasureText("A PRAIEIRA - CHICO SCIENCE", 20) - 30, 30, 20, MAROON);


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

    UnloadTexture(mapaBase);
    liberar_notas();
    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}