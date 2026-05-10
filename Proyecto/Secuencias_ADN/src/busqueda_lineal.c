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

            patrones[p].estado = NO_ENCONTRADO;
        }
    }
}

int main(int argc, char* argv[]) {

    parametros_t parametros;

    procesar_argumentos(argc, argv, &parametros);

    char *cadena_adn = reservar_vector(parametros.longitud_adn);

    patron_t *patrones =reservar_patrones(parametros.cantidad_patrones, parametros.longitud_patron);
 
    generar_adn(cadena_adn, parametros.longitud_adn);

    generar_patrones(
        patrones,
        parametros.longitud_patron,
        parametros.cantidad_patrones
    );

    buscar_patrones_lineal(cadena_adn, parametros.longitud_adn, patrones, parametros.cantidad_patrones
    );

    for (int i = 0; i < parametros.cantidad_patrones; i++) {

        printf(
            "Patron %d [%s] - Estado: [%d]",
            i,
            patrones[i].patron,
            patrones[i].estado
        );

        if (patrones[i].estado == COINCIDENCIA) {

            printf(
                " - Posicion encontrada: %d\n",
                patrones[i].encontrado_en
            );

        } else {

            printf(" - No encontrado\n");
        }
    }

    /* Liberar memoria */
    for (int i = 0; i < parametros.cantidad_patrones; i++) {
        free(patrones[i].patron);
    }

    free(patrones);
    free(cadena_adn);

    return 0;
}