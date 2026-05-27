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
static int streak = 0; // Contador de Combo

// Carrega as imagens de feedback apenas uma vez para otimizar memória
void InitPontos(void) {
    if (!texturasCarregadas) {
        texBom = LoadTexture("images/mensagens/bom.png");
        texMuitoBom = LoadTexture("images/mensagens/muito_bom.png");
        texPerfeito = LoadTexture("images/mensagens/perfeito.png");
        texErrou = LoadTexture("images/mensagens/errou.png");
        texturasCarregadas = true;
    }
}

// Reduz o tempo de exibição da mensagem na tela
void AtualizarPontosTempo(float deltaTime) {
    if (feedbackTimer > 0.0f) {
        feedbackTimer -= deltaTime;
    }
}

// Calcula a pontuação baseada na distância da nota até o alvo
void CalcularAcerto(GameContext *ctx, float dist, float distMax) {
    float precisao = (1.0f - (dist / distMax)) * 100.0f;
    int pontosBase = 0;
    
    // Define a pontuação base e a imagem a ser mostrada
    if (precisao >= 80.0f) {
        pontosBase = 100;
        tipoFeedback = 3; // Perfeito
    } else if (precisao >= 30.0f) {
        pontosBase = 75;
        tipoFeedback = 2; // Muito Bom
    } else {
        pontosBase = 50;
        tipoFeedback = 1; // Bom
    }
    
    streak++; // Incrementa o combo
    
    // Aplica multiplicador de pontuação
    float mult = 1.0f;
    if (streak >= 25) mult = 1.5f;
    else if (streak >= 20) mult = 1.4f;
    else if (streak >= 15) mult = 1.3f;
    else if (streak >= 10) mult = 1.2f;
    else if (streak >= 5) mult = 1.1f;
    
    ctx->pontuacao += (int)(pontosBase * mult);
    
    // Configura o tempo na tela
    feedbackTimer = 0.5f; 
    feedbackOffsetX = (float)(GetRandomValue(-30, 30));
    feedbackOffsetY = (float)(GetRandomValue(-20, 20));
}

// Penalidade por deixar a nota passar da tela
void RegistrarMiss(GameContext *ctx) {
    streak = 0; // Zera o combo
    ctx->pontuacao -= 10;
    if (ctx->pontuacao < 0) {
        ctx->pontuacao = 0;
    }
}

// Penalidade por apertar o botão na hora errada ou fazer spam
void RegistrarErroSpam(GameContext *ctx) {
    streak = 0; // Zera o combo
    ctx->pontuacao -= 20;
    
    if (ctx->pontuacao < 0) {
        ctx->pontuacao = 0;
    }
    
    tipoFeedback = 4; // Errou
    feedbackTimer = 0.5f;
    feedbackOffsetX = (float)(GetRandomValue(-30, 30));
    feedbackOffsetY = (float)(GetRandomValue(-20, 20));
}

// Desenha a mensagem de feedback e o contador de combo na tela
void DrawFeedback(GameContext *ctx) {
    if(feedbackTimer > 0.0f){
        Texture2D texDesenhar = texBom;

        if(tipoFeedback == 2) texDesenhar = texMuitoBom;
        else if(tipoFeedback == 3) texDesenhar = texPerfeito;
        else if(tipoFeedback == 4) texDesenhar = texErrou;

        float escalaTex = 1.0f;
        float basePosY = 0.0f;

        // Ajusta as escalas e posições
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

        // Centraliza a imagem
        Vector2 pos = {
            (float)ctx->largura / 2.0f - drawWidth / 2.0f + feedbackOffsetX,
            basePosY + feedbackOffsetY
        };

        DrawTextureEx(texDesenhar, pos, 0.0f, escalaTex, WHITE);
    }

    // Se o jogador tiver um combo ativo, desenha o número acima da capivara
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

// Limpa o feedback
void ResetarFeedback(void) {
    feedbackTimer = 0.0f;
    streak = 0;
}

// Descarrega as texturas
void UnloadPontos(void) {
    if (texturasCarregadas) {
        UnloadTexture(texBom);
        UnloadTexture(texMuitoBom);
        UnloadTexture(texPerfeito);
        UnloadTexture(texErrou); 
        texturasCarregadas = false;
    }
}