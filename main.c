#include "raylib.h"
#include "telas.h"

int main() {
    // CONFIGURAÇÃO INICIAL DA JANELA E TELA CHEIA
    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    InitWindow(800, 600, "Recife Beat");
    SetExitKey(0);
    
    int monitor = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    ToggleFullscreen();
    
    // INICIALIZAÇÃO DE ÁUDIO E DESEMPENHO
    InitAudioDevice();
    SetTargetFPS(60);

    // CONTEXTO DO JOGO E CARREGAMENTO DE RECURSOS GLOBAIS
    GameContext jogo;
    InitGameContext(&jogo); // Prepara as variáveis iniciais, aloca memória e define o estado como MENU

    jogo.fonteTitulo = LoadFontEx("fonts/PermanentMarker-Regular.ttf", 60, NULL, 0);
    jogo.mapaBase = LoadTexture("images/mapa_base.png");

    // Animação da Capivara
    jogo.framesCapivara[0] = LoadTexture("images/capivara_base1.png");
    jogo.framesCapivara[1] = LoadTexture("images/capivara_base2.png");
    jogo.framesCapivara[2] = LoadTexture("images/capivara_base3.png");
    jogo.framesCapivara[3] = LoadTexture("images/capivara_base4.png");

    // Animação do título no Menu Principal
    jogo.framesMenuTitulo[0] = LoadTexture("images/menu_titulo1.png");
    jogo.framesMenuTitulo[1] = LoadTexture("images/menu_titulo2.png");
    jogo.framesMenuTitulo[2] = LoadTexture("images/menu_titulo3.png");
    jogo.framesMenuTitulo[3] = LoadTexture("images/menu_titulo4.png");
    jogo.framesMenuTitulo[4] = LoadTexture("images/menu_titulo5.png");
    jogo.framesMenuTitulo[5] = LoadTexture("images/menu_titulo6.png");

    // LOOP PRINCIPAL DO JOGO 
    while (!WindowShouldClose() && !jogo.deveFechar) {
        switch (jogo.estadoAtual) {
            case MENU_PRINCIPAL: UpdateMenuPrincipal(&jogo); break;
            case MENU_MUSICAS:   UpdateMenuMusicas(&jogo); break;
            case MENU_AJUSTES:   UpdateMenuAjustes(&jogo); break;
            case TRANSICAO:      UpdateTransicao(&jogo); break;
            case JOGANDO:        UpdateJogando(&jogo); break;
            case INSERIR_NOME:   UpdateInserirNome(&jogo); break;
        }

        // RENDERIZAÇÃO 
        BeginDrawing();
        ClearBackground(BLACK);

        switch (jogo.estadoAtual) {
            case MENU_PRINCIPAL: DrawMenuPrincipal(&jogo); break;
            case MENU_MUSICAS:   DrawMenuMusicas(&jogo); break;
            case MENU_AJUSTES:   DrawMenuAjustes(&jogo); break;
            case TRANSICAO:      DrawTransicao(&jogo); break;
            case JOGANDO:        DrawJogando(&jogo); break;
            case INSERIR_NOME:   DrawInserirNome(&jogo); break;
        }

        EndDrawing();
    }

    // ENCERRAMENTO E LIMPEZA DE MEMÓRIA
    UnloadGameResources(&jogo); // Libera todas as texturas, sons e listas encadeadas
    CloseAudioDevice();
    CloseWindow();

    return 0;
}