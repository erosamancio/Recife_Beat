#ifndef PONTOS_H
#define PONTOS_H

#include "telas.h" 

void InitPontos(void);
void AtualizarPontosTempo(float deltaTime);
void CalcularAcerto(GameContext *ctx, float dist, float distMax);
void RegistrarMiss(GameContext *ctx);
void RegistrarErroSpam(GameContext *ctx);
void DrawFeedback(GameContext *ctx);
void ResetarFeedback(void);
void UnloadPontos(void);

#endif