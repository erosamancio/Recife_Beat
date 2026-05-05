#ifndef MUSICA_PRAIEIRA_H
#define MUSICA_PRAIEIRA_H

#include <raylib.h>

typedef struct {
    int tempo_ms;  
    int botao;     
    bool ativa;    
} Nota;

#define MAX_NOTAS 3000 
static Nota beatmap_a_praieira[MAX_NOTAS];
static int total_notas = 0;

void GerarFaseCompleta() {
    int tempo_base = 0;
    total_notas = 0;

    while (tempo_base < 216000) {
        
       
        if (tempo_base < 20000) {
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 0,    1, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 3000, 2, true};
        } 
        
        else if (tempo_base < 180000) {
           
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 0,    1, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 576,  2, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 1728, 2, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 2448, 1, true};
            
            if (tempo_base % 12000 < 6000) {
                beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 3600, 3, true}; 
                beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 4896, 0, true};
            } else {
                beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 4320, 0, true};
                beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 4896, 0, true};
                beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 4896, 1, true};
            }
        }
        
        else {
            
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 0,   1, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 200, 2, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 400, 1, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 600, 2, true};
            beatmap_a_praieira[total_notas++] = (Nota){tempo_base + 800, 3, true}; 
        }

        tempo_base += 6336; 
        if (total_notas > MAX_NOTAS - 20) break; 
    }
}

#endif