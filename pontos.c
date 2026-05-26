#include "pontos.h"

static Texture2D texBom = {0};
static Texture2D texMuitoBom = {0};
static Texture2D texPerfeito = {0};
static Texture2D texErrou = {0};
static bool texturasCarregadas = false;
static float feedbackTimer = 0.0f;
static int tipoFeedback = 0; 
static float feedbackOffsetX = 0.0f;
static float feedbackOffsetY = 0.0f;
static int streak = 0;

void InitPontos(void) {
    if (!texturasCarregadas) {
        texBom = LoadTexture("images/mensagens/bom.png");
        texMuitoBom = LoadTexture("images/mensagens/muito_bom.png");
        texPerfeito = LoadTexture("images/mensagens/perfeito.png");
        texErrou = LoadTexture("images/mensagens/errou.png");
        texturasCarregadas = true;
    }
}

void AtualizarPontosTempo(float deltaTime) {
    if (feedbackTimer > 0.0f) {
        feedbackTimer -= deltaTime;
    }
}

void CalcularAcerto(GameContext *ctx, float dist, float distMax) {
    float precisao = (1.0f - (dist / distMax)) * 100.0f;
    int pontosBase = 0;
    
    if (precisao >= 80.0f) {
        pontosBase = 100;
        tipoFeedback = 3; 
    } else if (precisao >= 30.0f) {
        pontosBase = 75;
        tipoFeedback = 2; 
    } else {
        pontosBase = 50;
        tipoFeedback = 1; 
    }
    
    streak++;
    
    float mult = 1.0f;
    if (streak >= 25) mult = 1.5f;
    else if (streak >= 20) mult = 1.4f;
    else if (streak >= 15) mult = 1.3f;
    else if (streak >= 10) mult = 1.2f;
    else if (streak >= 5) mult = 1.1f;
    
    ctx->pontuacao += (int)(pontosBase * mult);
    
    feedbackTimer = 0.5f; 
    feedbackOffsetX = (float)(GetRandomValue(-30, 30));
    feedbackOffsetY = (float)(GetRandomValue(-20, 20));
}

void RegistrarMiss(GameContext *ctx) {
    streak = 0;
    ctx->pontuacao -= 10;
    if (ctx->pontuacao < 0) {
        ctx->pontuacao = 0;
    }
}

void RegistrarErroSpam(GameContext *ctx) {
    streak = 0; 
    ctx->pontuacao -= 20;
    
    if (ctx->pontuacao < 0) {
        ctx->pontuacao = 0;
    }
    
    tipoFeedback = 4; 
    feedbackTimer = 0.5f;
    feedbackOffsetX = (float)(GetRandomValue(-30, 30));
    feedbackOffsetY = (float)(GetRandomValue(-20, 20));
}

void DrawFeedback(GameContext *ctx) {
    if(feedbackTimer > 0.0f){
        Texture2D texDesenhar = texBom;

        if(tipoFeedback == 2) texDesenhar = texMuitoBom;
        else if(tipoFeedback == 3) texDesenhar = texPerfeito;
        else if(tipoFeedback == 4) texDesenhar = texErrou;

        float escalaTex = 1.0f;
        float basePosY = 0.0f;

        if(tipoFeedback == 4) {
            float alturaErrou = 380.0f;
            escalaTex = alturaErrou / (float)texDesenhar.height;
            basePosY = 80.0f;
        } else {
            float alturaAcertos = 580.0f;
            if(tipoFeedback == 1) alturaAcertos = 770.0f;
            else if(tipoFeedback == 2) alturaAcertos = 780.0f;
            else if(tipoFeedback == 3) alturaAcertos = 780.0f;
            escalaTex = alturaAcertos / (float)texDesenhar.height;
            basePosY = -10.0f;
        }

        float drawWidth = texDesenhar.width * escalaTex;

        Vector2 pos = {
            (float)ctx->largura / 2.0f - drawWidth / 2.0f + feedbackOffsetX,
            basePosY + feedbackOffsetY
        };

        DrawTextureEx(texDesenhar, pos, 0.0f, escalaTex, WHITE);
    }

    if (streak > 0) {
        float escalaCap = 0.3f;
        float hCap = ctx->framesCapivara[ctx->frameCapivara].height * escalaCap;
        float wCap = ctx->framesCapivara[ctx->frameCapivara].width * escalaCap;
        float capX = ctx->deslocamentoX + 550.0f;
        float capY = ctx->altura / 2.0f - (hCap / 2.0f);

        const char* streakText = TextFormat("%d", streak);
        Vector2 textSize = MeasureTextEx(ctx->fonteTitulo, streakText, 50, 2);

        Vector2 posText = {
            capX + (wCap / 2.0f) - (textSize.x / 2.0f),
            capY - textSize.y - 15.0f
        };

        DrawTextEx(ctx->fonteTitulo, streakText, posText, 50, 2, GOLD);
    }
}

void ResetarFeedback(void) {
    feedbackTimer = 0.0f;
    streak = 0;
}

void UnloadPontos(void) {
    if (texturasCarregadas) {
        UnloadTexture(texBom);
        UnloadTexture(texMuitoBom);
        UnloadTexture(texPerfeito);
        UnloadTexture(texErrou); 
        texturasCarregadas = false;
    }
}