#include "../include/cadenas.h"
#include "../include/busqueda_lineal.h"
#include "../include/parametros.h"


void buscar_patrones_lineal(const char* cadena_adn, int longitud_cadena_adn, patron_t* patrones, int cantidad_patrones) {

    for (int p = 0; p < cantidad_patrones; p++) {

        char* patron = patrones[p].patron;
        int longitud_patron = patrones[p].longitud;

        int coincidencia_en = -1;

        int fin_busqueda = longitud_cadena_adn - longitud_patron;

        for (int i = 0; i <= fin_busqueda; i++) {

            int j;

            for (j = 0; j < longitud_patron; j++) {

                if (cadena_adn[i + j] != patron[j]) {
                    break;
                }
            }

            if (j == longitud_patron) {

                coincidencia_en = i;
                break;
            }
        }

        if (coincidencia_en != -1) {

            patrones[p].encontrado_en = coincidencia_en;
            patrones[p].estado = COINCIDENCIA;

        } else {

            patrones[p].encontrado_en = NO_ENCONTRADO;
            patrones[p].estado = NO_ENCONTRADO;
        }
    }
}
