#include "../include/cadenas.h"
#include "../include/busqueda_MPI.h"
#include "../include/parametros.h"


void buscar_patrones_MPI(const char* cadena_adn, int longitud_cadena_adn, patron_t* patrones) {
        char* patron = patrones->patron;
        int longitud_patron = patrones->longitud;

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

            patrones->encontrado_en = coincidencia_en;
            patrones->estado = COINCIDENCIA;

        } else {

            patrones->estado = NO_ENCONTRADO;
        }
}

