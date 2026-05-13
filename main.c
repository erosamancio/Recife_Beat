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
    JOGANDO,
    INSERIR_NOME
} EstadoJogo;

char nomeInput[12] = "\0"; 
int contLetras = 0;

typedef struct {
    char nome[10];
    int pontos;
} Score;

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

#define NUM_MUSICAS 5

const char* titulosMusicas[NUM_MUSICAS] = {
    "A Praieira - Chico Science",
    "Anunciacao - Alceu Valenca",
    "Frevo Mulher - Amelinha",
    "Leao do Norte - Lenine",
    "Voltei Recife - Alceu Valenca"
};

const char* arquivosAudio[NUM_MUSICAS] = {
    "audio/a_praieira.ogg",
    "audio/anunciacao.ogg",
    "audio/frevo_mulher.ogg",
    "audio/leao_do_norte.ogg",
    "audio/voltei_recife.ogg"
};

const char* arquivosMapa[NUM_MUSICAS] = {
    "mapa_praieira.txt",
    "mapa_anunciacao.txt",
    "mapa_frevo_mulher.txt",
    "mapa_leao_do_norte.txt",
    "mapa_voltei_recife.txt"
};

#define MAX_SCORES 5

const char* arquivosRanking[NUM_MUSICAS] = {
    "ranking_praieira.txt",
    "ranking_anunciacao.txt",
    "ranking_frevo_mulher.txt",
    "ranking_leao_do_norte.txt",
    "ranking_voltei_recife.txt"
};

const char* ranking_atual = "ranking_praieira.txt";

// ==========================================
//   QUICKSORT 
// ==========================================
void trocar(Score* a, Score* b) {
    Score temp = *a;
    *a = *b;
    *b = temp;
}

int particao(Score arr[], int baixo, int alto){
    int pivo = arr[alto].pontos;
    int i = (baixo - 1);

    for(int j = baixo; j <= alto - 1; j++){
        if(arr[j].pontos >= pivo){
            i++;
            trocar(&arr[i], &arr[j]);
        }
    }
    trocar(&arr[i + 1], &arr[alto]);
    return (i + 1);
}

void quicksort(Score arr[], int baixo, int alto){
    if(baixo < alto){
        int pi = particao(arr, baixo, alto);
        quicksort(arr, baixo, pi - 1);
        quicksort(arr, pi + 1, alto);
    }
}

void salvar_score(const char* arquivo, const char* nome_jogador, int nova_pontuacao) {
    if (nova_pontuacao == 0) return;

    Score ranking[MAX_SCORES + 1];
    int qtd = 0;
    FILE *f = fopen(arquivo, "r");
    if(f != NULL){
        while(fscanf(f, "%s %d", ranking[qtd].nome, &ranking[qtd].pontos) != EOF){
            qtd++;
            if(qtd >= MAX_SCORES)break;
        }
        fclose(f);
    }

    TextCopy(ranking[qtd].nome, nome_jogador); 
    ranking[qtd].pontos = nova_pontuacao;
    qtd++;

    quicksort(ranking, 0, qtd - 1);

    f = fopen(arquivo, "w");
    if(f != NULL){
        int limite;
        if(qtd > MAX_SCORES){
            limite = MAX_SCORES;
        }else{ 
            limite = qtd;
        }
        for(int i = 0; i < limite; i++) {
            fprintf(f, "%s %d\n", ranking[i].nome, ranking[i].pontos);
        }
        fclose(f);
    }
}

Score rankingTela[MAX_SCORES];
int qtdRankingTela = 0;
void carregar_ranking_tela(const char* arquivo){
    qtdRankingTela = 0;
    FILE *f = fopen(arquivo, "r");
    if(f != NULL){
        while(fscanf(f, "%s %d", rankingTela[qtdRankingTela].nome, &rankingTela[qtdRankingTela].pontos) != EOF){
            qtdRankingTela++;
            if(qtdRankingTela >= MAX_SCORES) break;
        }
        fclose(f);
    }
}

// ==========================================
// FUNÇÃO PRINCIPAL
// ==========================================
int main(){
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(800, 600, "Recife Beat");
    SetExitKey(0);
    int monitor = GetCurrentMonitor();
    LARGURA_TELA = GetMonitorWidth(monitor);
    ALTURA_TELA = GetMonitorHeight(monitor);

    SetWindowSize(LARGURA_TELA, ALTURA_TELA);
    ToggleFullscreen();
    Y_ALVO = ALTURA_TELA - 150; 

    InitAudioDevice();

    Texture2D mapaBase = LoadTexture("images/mapa_base.png");
    
    // ==========================================
    // CARREGAMENTO DA FONTE CUSTOMIZADA
    // ==========================================
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
    int opcaoMusica = 0;
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
                        opcaoMusica = 0;
                        carregar_ranking_tela(arquivosRanking[opcaoMusica]);
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
                if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
                    opcaoMusica = (opcaoMusica + 1) % NUM_MUSICAS;
                    carregar_ranking_tela(arquivosRanking[opcaoMusica]);
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
                    opcaoMusica = (opcaoMusica - 1 + NUM_MUSICAS) % NUM_MUSICAS;
                    carregar_ranking_tela(arquivosRanking[opcaoMusica]);
                }

                if(IsKeyPressed(KEY_ENTER)){
                    liberar_notas(); 
                    UnloadMusicStream(musica);
                    musica = LoadMusicStream(arquivosAudio[opcaoMusica]);
                    mapa_atual = arquivosMapa[opcaoMusica]; 
                    ranking_atual = arquivosRanking[opcaoMusica];
                    carregar_mapa(mapa_atual);

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
                    StopMusicStream(musica); 
                    if(!modoEditor && pontuacao > 0){
                        estadoAtual = INSERIR_NOME;
                        nomeInput[0] = '\0';
                        contLetras = 0;
                    }else{
                        estadoAtual = MENU_PRINCIPAL;
                        pontuacao = 0;
                        TextCopy(mensagem_feedback, "");
                        alphaTransicao = 1.0f;
                        modoEditor = false;
                    }
                    resetar_notas();
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
            case INSERIR_NOME:
            {
                int tecla = GetCharPressed();
                while (tecla > 0) {
                    if ((tecla >= 32) && (tecla <= 125) && (contLetras < 15)) {
                        nomeInput[contLetras] = (char)tecla;
                        nomeInput[contLetras + 1] = '\0';
                        contLetras++;
                    }
                    tecla = GetCharPressed(); 
                }

                if (IsKeyPressed(KEY_BACKSPACE)) {
                    contLetras--;
                    if (contLetras < 0) contLetras = 0;
                    nomeInput[contLetras] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER) && contLetras > 0) {
                    for (int i = 0; i < contLetras; i++) {
                        if (nomeInput[i] == ' ') nomeInput[i] = '_';
                    }

                    salvar_score(ranking_atual, nomeInput, pontuacao); 
                    
                    estadoAtual = MENU_PRINCIPAL;
                    pontuacao = 0;
                    TextCopy(mensagem_feedback, "");
                    alphaTransicao = 1.0f;
                }
            }
            break;
        }

        // ==========================================
        // LÓGICA DE DESENHO (DRAW)
        // ==========================================
        BeginDrawing();
        ClearBackground(BLACK);

        if(estadoAtual == MENU_PRINCIPAL){
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "RECIFE BEAT", 60, 2);
            DrawTextEx(fonteTitulo, "RECIFE BEAT", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 4}, 60, 2, WHITE);

            DrawText(opcaoMenu == 0 ? "> JOGAR <" : "JOGAR", LARGURA_TELA / 2 - 80, ALTURA_TELA / 2, 40, opcaoMenu == 0 ? YELLOW : GRAY);
            DrawText(opcaoMenu == 1 ? "> AJUSTES <" : "AJUSTES", LARGURA_TELA / 2 - 110, ALTURA_TELA / 2 + 70, 40, opcaoMenu == 1 ? YELLOW : GRAY);
            DrawText(opcaoMenu == 2 ? "> SAIR <" : "SAIR", LARGURA_TELA / 2 - 60, ALTURA_TELA / 2 + 140, 40, opcaoMenu == 2 ? YELLOW : GRAY);
        }
        else if(estadoAtual == MENU_MUSICAS){
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "SELECIONE A MUSICA", 50, 2);
            DrawTextEx(fonteTitulo, "SELECIONE A MUSICA", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 6}, 50, 2, WHITE);

            for(int i = 0; i < NUM_MUSICAS; i++){
                Color corTexto = (i == opcaoMusica) ? YELLOW : GRAY;
                const char* textoMusica = (i == opcaoMusica) ? TextFormat("> %s <", titulosMusicas[i]) : titulosMusicas[i];
                
                int larguraTexto = MeasureText(textoMusica, 40);
                DrawText(textoMusica, (LARGURA_TELA / 2) - larguraTexto / 2 - 200, ALTURA_TELA / 3 + (i * 60), 40, corTexto);
            }
            int posXRanking = LARGURA_TELA - 450; 
            int posYRanking = ALTURA_TELA / 3;
            
            DrawText("TOP 5 - RANKING", posXRanking, posYRanking - 50, 30, GOLD);

            if(qtdRankingTela == 0){
                DrawText("Sem pontuacoes ainda...", posXRanking, posYRanking, 20, LIGHTGRAY);
                DrawText("Jogue e seja o primeiro!", posXRanking, posYRanking + 30, 20, DARKGRAY);
            }else{
                for(int i = 0; i < qtdRankingTela; i++){
                    const char* linhaRank = TextFormat("%d. %s - %06d", i + 1, rankingTela[i].nome, rankingTela[i].pontos);
                    
                    Color corRank = WHITE;
                    if (i == 0) corRank = GOLD;
                    else if (i == 1) corRank = LIGHTGRAY;
                    else if (i == 2) corRank = ORANGE;

                    DrawText(linhaRank, posXRanking, posYRanking + (i * 35), 25, corRank);
                }
            }

            DrawText("Pressione ESC para voltar", 30, ALTURA_TELA - 50, 20, GRAY);
        }
        else if (estadoAtual == MENU_AJUSTES) {
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "AJUSTES DE AUDIO", 50, 2);
            DrawTextEx(fonteTitulo, "AJUSTES DE AUDIO", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 4}, 50, 2, WHITE);

            DrawText(TextFormat("Volume Geral: < %0.0f%% >", volumeGeral * 100), LARGURA_TELA / 2 - 180, ALTURA_TELA / 2, 40, YELLOW);
            DrawText("Pressione ESC para voltar", 30, ALTURA_TELA - 50, 20, GRAY);
        }
        else if (estadoAtual == JOGANDO || estadoAtual == TRANSICAO){
            
            Rectangle source = {0.0f, 0.0f, (float)mapaBase.width, (float)mapaBase.height};
            Rectangle dest = {0.0f, 0.0f, (float)LARGURA_TELA, (float)ALTURA_TELA};
            DrawTexturePro(mapaBase, source, dest, (Vector2){ 0, 0 }, 0.0f, WHITE);

            // DESENHA A CAPIVARA
            float escalaCapivara = 0.3f; 
            float alturaCapivara = framesCapivara[frameAtualCapivara].height * escalaCapivara;
            float posCapivaraX = deslocamentoX + 550; 
            float posCapivaraY = (ALTURA_TELA / 2) - (alturaCapivara / 2);

            DrawTextureEx(
                framesCapivara[frameAtualCapivara], 
                (Vector2){ posCapivaraX, posCapivaraY }, 
                0.0f, 
                escalaCapivara, 
                WHITE
            );

            float y_horizonte = ALTURA_TELA / 2.2f;
            float centro_pistas = deslocamentoX + 300.0f;
            float progresso_tela_toda = (ALTURA_TELA - y_horizonte) / (Y_ALVO - y_horizonte);
            float escala_tela_toda = 0.2f + (0.8f * progresso_tela_toda);

            Color corFundoPista = ColorAlpha(BLACK, 0.6f); // 60% de opacidade
            for(int i = 0; i < 4; i++){
                float x_base_esq = deslocamentoX + i * 150;
                float x_topo_esq = centro_pistas + (x_base_esq - centro_pistas) * 0.2f;
                float x_fim_esq = centro_pistas + (x_base_esq - centro_pistas) * escala_tela_toda;

                float x_base_dir = deslocamentoX + (i + 1) * 150;
                float x_topo_dir = centro_pistas + (x_base_dir - centro_pistas) * 0.2f;
                float x_fim_dir = centro_pistas + (x_base_dir - centro_pistas) * escala_tela_toda;

                Vector2 topoEsq = {x_topo_esq, y_horizonte};
                Vector2 baseEsq = {x_fim_esq, ALTURA_TELA};
                Vector2 baseDir = {x_fim_dir, ALTURA_TELA};
                Vector2 topoDir = {x_topo_dir, y_horizonte};

                DrawTriangle(topoEsq, baseEsq, baseDir, corFundoPista);
                DrawTriangle(topoEsq, baseDir, topoDir, corFundoPista);
            }

            for(int i = 0; i <= 4; i++){
                float x_base_linha = deslocamentoX + i * 150;
                float x_topo_linha = centro_pistas + (x_base_linha - centro_pistas) * 0.2f; 
                float x_fim_linha = centro_pistas + (x_base_linha - centro_pistas) * escala_tela_toda;
                
                DrawLineEx((Vector2){x_topo_linha, y_horizonte}, (Vector2){x_fim_linha, ALTURA_TELA}, 4.0f, ColorAlpha(LIGHTGRAY, 0.5f));
            }

            for(int i = 0; i < 4; i++){
                float x_base_pista = deslocamentoX + 75 + i * 150;
                DrawCircleLines(x_base_pista, Y_ALVO, RAIO_NOTA + 2, BLACK);
                
                const char* teclasStr[] = {"C", "V", "N", "M"};
                DrawText(teclasStr[i], x_base_pista - 10, Y_ALVO - 15, 30, WHITE);
            }

            Nota *atual = inicio;
            float tempo_atual_ms = GetMusicTimePlayed(musica) * 1000.0f;

            while(atual != NULL) {
                if(atual->ativa) {
                    float y_pos = Y_ALVO - (atual->tempo_ms - tempo_atual_ms) * VELOCIDADE;
                    if(y_pos > ALTURA_TELA + 50){
                        atual->ativa = false;
                    }

                    if(y_pos > y_horizonte && y_pos < ALTURA_TELA){
                        float progresso = (y_pos - y_horizonte) / (Y_ALVO - y_horizonte);
                        if(progresso < 0.0f)
                         progresso = 0.0f;
                        float escala_perspectiva = 0.2f + (0.8f * progresso);
                        float x_base_nota = deslocamentoX + 75 + atual->botao * 150;
                        
                        float x_atual = centro_pistas + (x_base_nota - centro_pistas) * escala_perspectiva;
                        float raio_atual = RAIO_NOTA * escala_perspectiva;

                        Color corNota = (tempo_atual_ms > 180000) ? ORANGE : MAROON;
                        DrawCircle((int)x_atual, (int)y_pos, raio_atual, corNota);
                    }
                }
                atual = atual->prox;
            }

            DrawText(TextFormat("PONTOS: %06d", pontuacao), 30, 30, 30, WHITE);
            DrawText(mensagem_feedback, LARGURA_TELA / 2 - MeasureText(mensagem_feedback, 40) / 2, 80, 40, GOLD);
            
            Vector2 tamanhoGameplay = MeasureTextEx(fonteTitulo, titulosMusicas[opcaoMusica], 30, 2);
            DrawTextEx(fonteTitulo, titulosMusicas[opcaoMusica], (Vector2){LARGURA_TELA - tamanhoGameplay.x - 30, 30}, 30, 2, WHITE);

            if(modoEditor){
                DrawRectangle(0, 0, LARGURA_TELA, 50, Fade(RED, 0.8f));
                DrawText("MODO EDITOR | TOQUE PARA GRAVAR | S: SALVAR | DELETE: LIMPAR", 30, 15, 20, WHITE);
            }

            if(estadoAtual == TRANSICAO){
                DrawRectangle(0, 0, LARGURA_TELA, ALTURA_TELA, Fade(BLACK, alphaTransicao));
            }
        }else if (estadoAtual == INSERIR_NOME){
            Vector2 tamanhoTitulo = MeasureTextEx(fonteTitulo, "FIM DE JOGO", 60, 2);
            DrawTextEx(fonteTitulo, "FIM DE JOGO", (Vector2){LARGURA_TELA / 2 - tamanhoTitulo.x / 2, ALTURA_TELA / 4}, 60, 2, WHITE);

            DrawText(TextFormat("PONTUACAO: %d", pontuacao), LARGURA_TELA / 2 - MeasureText(TextFormat("PONTUACAO: %d", pontuacao), 40) / 2, ALTURA_TELA / 2 - 60, 40, GOLD);
            DrawText("DIGITE SEU NOME:", LARGURA_TELA / 2 - MeasureText("DIGITE SEU NOME:", 30) / 2, ALTURA_TELA / 2 + 20, 30, GRAY);
            
            DrawRectangle(LARGURA_TELA / 2 - 200, ALTURA_TELA / 2 + 70, 400, 50, LIGHTGRAY);
            DrawRectangleLines(LARGURA_TELA / 2 - 200, ALTURA_TELA / 2 + 70, 400, 50, DARKGRAY);
            
            DrawText(nomeInput, LARGURA_TELA / 2 - MeasureText(nomeInput, 30) / 2, ALTURA_TELA / 2 + 80, 30, BLACK);

            if (((int)(GetTime() * 2)) % 2 == 0 && contLetras < 15) {
                DrawText("_", LARGURA_TELA / 2 + MeasureText(nomeInput, 30) / 2 + 5, ALTURA_TELA / 2 + 80, 30, MAROON);
            }

            DrawText("Pressione ENTER para salvar", LARGURA_TELA / 2 - MeasureText("Pressione ENTER para salvar", 20) / 2, ALTURA_TELA - 100, 20, DARKGRAY);
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
    UnloadFont(fonteTitulo);
    
    liberar_notas();
    UnloadMusicStream(musica);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}