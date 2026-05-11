#include <stdio.h>
#include <assert.h>
#include "../include/busqueda_lineal.h"

int main(){
    char *test_secuencias[] = {"ATGCGT", "ATGCGT", "ATGCGT", "AAAAAA"};
    char *test_patron[] = {"ATG", "CGT", "AAA", "AAA"};
    patron_t patrones[4];
    for(int i = 0; i < 4; i++){
        patrones[i] = (patron_t){
        .patron = test_patron[i],
        .longitud = sizeof(test_patron[i]) - 1,
        .encontrado_en = NO_ENCONTRADO,
        .estado = EN_COLA};
    }
    buscar_patrones_lineal(test_secuencias[0], 6, &patrones[0], 1);
    assert(patrones->encontrado_en == 0);

    buscar_patrones_lineal(test_secuencias[1], 6, &patrones[1], 1);
    assert(patrones->encontrado_en == 3);

    buscar_patrones_lineal(test_secuencias[2], 6, &patrones[2], 1);
    assert(patrones->encontrado_en == NO_ENCONTRADO);

    buscar_patrones_lineal(test_secuencias[3], 6, &patrones[3], 1);
    assert(patrones->encontrado_en == 0);

}

