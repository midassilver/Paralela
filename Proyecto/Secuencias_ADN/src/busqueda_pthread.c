#include "../include/busqueda_pthread.h"
#include "../include/parametros.h"
#include "../include/cadenas.h"

void* trabajador_hilo(void* arg) {

    argumentos_hilo_t* datos = (argumentos_hilo_t*) arg;

    for (int p = datos->indice_inicio; p < datos->indice_fin; p++) {

        patron_t* patron_estructura = &datos->patrones[p];

        char* patron = patron_estructura->patron;

        int longitud_patron = patron_estructura->longitud;

        int coincidencia_en = NO_ENCONTRADO;

        int fin_busqueda = datos->longitud_cadena_adn - longitud_patron;

        for (int i = 0; i <= fin_busqueda; i++) {

            int j;

            for (j = 0; j < longitud_patron; j++) {

                if (datos->cadena_adn[i + j] != patron[j]) {
                    break;
                }
            }

            if (j == longitud_patron) {

                coincidencia_en = i;

                break;
            }
        }

        if (coincidencia_en != NO_ENCONTRADO) {

            patron_estructura->encontrado_en = coincidencia_en;
            patron_estructura->estado = ENCONTRADO;

        } else {

            patron_estructura->estado = NO_ENCONTRADO;
        }
    }

    return NULL;
}
void buscar_patrones_pthread(
    const char* cadena_adn,
    int longitud_adn,
    patron_t* patrones,
    int cantidad_patrones,
    int cantidad_hilos
) {

    pthread_t hilos[cantidad_hilos];
    argumentos_hilo_t argumentos[cantidad_hilos];

    int patrones_por_hilo = cantidad_patrones / cantidad_hilos;

    for (int i = 0; i < cantidad_hilos; i++) {

        argumentos[i].cadena_adn = cadena_adn;

        argumentos[i].longitud_cadena_adn = longitud_adn;

        argumentos[i].patrones = patrones;

        argumentos[i].indice_inicio = i * patrones_por_hilo;

        argumentos[i].indice_fin =
            (i == cantidad_hilos - 1)
            ? cantidad_patrones
            : (i + 1) * patrones_por_hilo;

        pthread_create(
            &hilos[i],
            NULL,
            trabajador_hilo,
            &argumentos[i]
        );
    }

    for (int i = 0; i < cantidad_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }
}