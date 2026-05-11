#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../include/busqueda_lineal.h"
#include "../include/busqueda_pthread.h"

int main(){

    char *test_secuencias[] = {
        "ATGCGT",
        "ATGCGT",
        "ATGCGT",
        "AAAAAA"
    };

    char *test_patron[] = {
        "ATG",
        "CGT",
        "AAA",
        "AAA"
    };

    patron_t patrones[4];

    for(int i = 0; i < 4; i++){

        patrones[i] = (patron_t){
            .patron = test_patron[i],
            .longitud = strlen(test_patron[i]),
            .encontrado_en = NO_ENCONTRADO,
            .estado = EN_COLA
        };
    }

    buscar_patrones_lineal(test_secuencias[0], 6, &patrones[0], 1);
    assert(patrones[0].encontrado_en == 0);

    buscar_patrones_lineal(test_secuencias[1], 6, &patrones[1], 1);
    assert(patrones[1].encontrado_en == 3);

    buscar_patrones_lineal(test_secuencias[2], 6, &patrones[2], 1);
    assert(patrones[2].encontrado_en == NO_ENCONTRADO);

    buscar_patrones_lineal(test_secuencias[3], 6, &patrones[3], 1);
    assert(patrones[3].encontrado_en == 0);

    char* adn = reservar_vector(100000);
    patron_t* patrones_nuevos = reservar_patrones(50, 5);
    generar_adn(adn, 100000);
    generar_patrones(patrones_nuevos, 5, 50);

    patron_t* patrones_secuenciales = malloc(sizeof(patron_t) * 50);
    patron_t* patrones_pthreads = malloc(sizeof(patron_t) * 50);

    for(int i = 0; i < 50; i++){

        patrones_secuenciales[i].longitud = patrones_nuevos[i].longitud;
        patrones_secuenciales[i].encontrado_en = patrones_nuevos[i].encontrado_en;
        patrones_secuenciales[i].estado = patrones_nuevos[i].estado;

        patrones_secuenciales[i].patron = malloc(patrones_nuevos[i].longitud + 1);
        strcpy(patrones_secuenciales[i].patron, patrones_nuevos[i].patron);

        patrones_pthreads[i].longitud = patrones_nuevos[i].longitud;
        patrones_pthreads[i].encontrado_en = patrones_nuevos[i].encontrado_en;
        patrones_pthreads[i].estado = patrones_nuevos[i].estado;

        patrones_pthreads[i].patron = malloc(patrones_nuevos[i].longitud + 1);
        strcpy(patrones_pthreads[i].patron, patrones_nuevos[i].patron);
    }
    
    buscar_patrones_lineal(adn, 100000, patrones_secuenciales, 50);
    buscar_patrones_pthread(adn, 100000, patrones_pthreads, 50, 5);
    for(int i = 0; i < 50; i++){
        assert(
            patrones_secuenciales[i].encontrado_en ==
            patrones_pthreads[i].encontrado_en
        );
        assert(
            patrones_secuenciales[i].estado ==
            patrones_pthreads[i].estado
        );
    }

    printf("Tests pasados\n");

    return 0;
}