#ifndef CONTROLES_H
#define CONTROLES_H

#include "raylib.h"
#include <stdbool.h>

bool ControleCimaPressionado(void);
bool ControleBaixoPressionado(void);
bool ControleEsquerdaPressionado(void);
bool ControleDireitaPressionado(void);

bool ControleSelecionarPressionado(void);
bool ControleVoltarPressionado(void);
bool ControleRankingPressionado(void);

bool ControlePistaPressionada(int pista, int tecla);

#endif