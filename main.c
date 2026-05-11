#include "raylib.h"
#include "musica_praieira.h"
#include <math.h>
#include <stdlib.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define Y_ALVO 500
#define RAIO_NOTA 30
#define VELOCIDADE 0.5f

int pontuacao = 0;
char mensagem_feedback[20] = "";

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

int main() {

    InitWindow(LARGURA_TELA, ALTURA_TELA, "Recife Beat - A Praieira");

    InitAudioDevice();

    GerarFaseCompleta();

    Music musica = LoadMusicStream("audio/a_praieira.ogg");

    PlayMusicStream(musica);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {

        UpdateMusicStream(musica);

        float tempo_atual_ms = GetMusicTimePlayed(musica) * 1000.0f;

        if (tempo_atual_ms >= 216000)
            break;

        int teclas[] = { KEY_LEFT, KEY_UP, KEY_DOWN, KEY_RIGHT };

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

        BeginDrawing();

            ClearBackground(RAYWHITE);

            for (int i = 0; i < 4; i++) {

                DrawRectangle(150 + i * 150, 0, 100, ALTURA_TELA, LIGHTGRAY);

                DrawCircleLines(
                    200 + i * 150,
                    Y_ALVO,
                    RAIO_NOTA + 2,
                    BLACK
                );
            }

            Nota *atual = inicio;

            while (atual != NULL) {

                if (atual->ativa) {

                    float y_pos = Y_ALVO - (atual->tempo_ms - tempo_atual_ms) * VELOCIDADE;

                    if (y_pos > ALTURA_TELA + 50) {
                        atual->ativa = false;
                    }

                    if (y_pos > -50 && y_pos < ALTURA_TELA) {

                        Color corNota;

                        if (tempo_atual_ms > 180000)
                            corNota = ORANGE;
                        else
                            corNota = MAROON;

                        DrawCircle(
                            200 + atual->botao * 150,
                            (int)y_pos,
                            RAIO_NOTA,
                            corNota
                        );
                    }
                }

                atual = atual->prox;
            }

            DrawText(
                TextFormat("PONTOS: %06d", pontuacao),
                20,
                20,
                25,
                DARKGRAY
            );

            DrawText(
                mensagem_feedback,
                350,
                50,
                30,
                GOLD
            );

            DrawText(
                "A PRAIEIRA - CHICO SCIENCE",
                250,
                15,
                20,
                MAROON
            );

        EndDrawing();
    }

    liberar_notas();

    UnloadMusicStream(musica);

    CloseAudioDevice();

    CloseWindow();

    return 0;
}