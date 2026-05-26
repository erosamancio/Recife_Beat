#include "controles.h"

#define CONTROLE_ID 0
#define LIMIAR_ANALOGICO 0.55f

static bool segurandoCima = false;
static bool segurandoBaixo = false;
static bool segurandoEsquerda = false;
static bool segurandoDireita = false;

static bool PressionouAnalogico(bool ativoAgora, bool *segurando) {
    bool pressionou = ativoAgora && !(*segurando);
    *segurando = ativoAgora;
    return pressionou;
}

bool ControleCimaPressionado(void) {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) return true;

    if (IsGamepadAvailable(CONTROLE_ID)) {
        if (IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_LEFT_FACE_UP)) return true;

        float eixoY = GetGamepadAxisMovement(CONTROLE_ID, GAMEPAD_AXIS_LEFT_Y);
        if (PressionouAnalogico(eixoY < -LIMIAR_ANALOGICO, &segurandoCima)) return true;
    }

    return false;
}

bool ControleBaixoPressionado(void) {
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) return true;

    if (IsGamepadAvailable(CONTROLE_ID)) {
        if (IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) return true;

        float eixoY = GetGamepadAxisMovement(CONTROLE_ID, GAMEPAD_AXIS_LEFT_Y);
        if (PressionouAnalogico(eixoY > LIMIAR_ANALOGICO, &segurandoBaixo)) return true;
    }

    return false;
}

bool ControleEsquerdaPressionado(void) {
    if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) return true;

    if(IsGamepadAvailable(CONTROLE_ID)) {
        if(IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) return true;

        float eixoX = GetGamepadAxisMovement(CONTROLE_ID, GAMEPAD_AXIS_LEFT_X);
        if(PressionouAnalogico(eixoX < -LIMIAR_ANALOGICO, &segurandoEsquerda)) return true;
    }

    return false;
}

bool ControleDireitaPressionado(void) {
    if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) return true;

    if(IsGamepadAvailable(CONTROLE_ID)) {
        if(IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) return true;

        float eixoX = GetGamepadAxisMovement(CONTROLE_ID, GAMEPAD_AXIS_LEFT_X);
        if(PressionouAnalogico(eixoX > LIMIAR_ANALOGICO, &segurandoDireita)) return true;
    }

    return false;
}

bool ControleSelecionarPressionado(void) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) return true;

    if(IsGamepadAvailable(CONTROLE_ID)){
        if(IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) return true;
    }

    return false;
}

bool ControleVoltarPressionado(void) {
    if(IsKeyPressed(KEY_ESCAPE)) return true;

    if(IsGamepadAvailable(CONTROLE_ID)) {
        if(IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) return true;
    }

    return false;
}

bool ControleRankingPressionado(void) {
    if(IsKeyPressed(KEY_R)) return true;

    if(IsGamepadAvailable(CONTROLE_ID)){
        if(IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) return true;
    }

    return false;
}

bool ControlePistaPressionada(int pista, int tecla) {
    if(IsKeyPressed(tecla)) return true;

    if(IsGamepadAvailable(CONTROLE_ID)){
        if(pista == 0 && IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) return true;
        if(pista == 1 && IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) return true;
        if(pista == 2 && IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) return true;
        if(pista == 3 && IsGamepadButtonPressed(CONTROLE_ID, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) return true;
    }

    return false;
}