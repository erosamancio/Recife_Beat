#include "pontos.h"

static Texture2D texBom = {0};
static Texture2D texMuitoBom = {0};
static Texture2D texPerfeito = {0};
static bool texturasCarregadas = false;
static float feedbackTimer = 0.0f;
static int tipoFeedback = 0; 
static float feedbackOffsetX = 0.0f;
static float feedbackOffsetY = 0.0f;

void InitPontos(void) {
    if (!texturasCarregadas) {
        texBom = LoadTexture("images/mensagens/bom.png");
        texMuitoBom = LoadTexture("images/mensagens/muito_bom.png");
        texPerfeito = LoadTexture("images/mensagens/perfeito.png");
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
    feedbackOffsetX = (float)(GetRandomValue(-30, 30));
    feedbackOffsetY = (float)(GetRandomValue(-20, 20));
}

void DrawFeedback(GameContext *ctx) {
    if (feedbackTimer > 0.0f) {
        Texture2D texDesenhar = texBom;
        if (tipoFeedback == 2) texDesenhar = texMuitoBom;
        else if (tipoFeedback == 3) texDesenhar = texPerfeito;
        
        float escalaTex = 380.0f / (float)texDesenhar.height;
        float drawWidth = texDesenhar.width * escalaTex;
        
        Vector2 pos = { (float)ctx->largura / 2.0f - drawWidth / 2.0f + feedbackOffsetX, 80.0f + feedbackOffsetY };
        DrawTextureEx(texDesenhar, pos, 0.0f, escalaTex, WHITE);
    }
}

void ResetarFeedback(void) {
    feedbackTimer = 0.0f;
}

void UnloadPontos(void) {
    if (texturasCarregadas) {
        UnloadTexture(texBom);
        UnloadTexture(texMuitoBom);
        UnloadTexture(texPerfeito);
        texturasCarregadas = false;
    }
}