#include "../include/cadenas_pthread.h"
#include "../include/parametros.h"

void* trabajador_hilo(void* arg) {

    argumentos_hilo_t* datos = (argumentos_hilo_t*) arg;

    for (int p = datos->indice_inicio; p < datos->indice_final; p++) {

        patron_t* patron_estructura = &datos->patrones[p];

        char* patron = patron_estructura->patron;

        int longitud_patron = patron_estructura->longitud;

        int coincidencia_en = NO_ENCONTRADO;

        int fin_busqueda = datos->longitud_adn - longitud_patron;


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

/*
    Función principal de búsqueda paralela con pthreads
*/
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

        argumentos[i].longitud_adn = longitud_adn;

        argumentos[i].patrones = patrones;

        argumentos[i].indice_inicio = i * patrones_por_hilo;

        argumentos[i].indice_final =
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


int main(int argc, char* argv[]) {

    params_t parametros;

    /*
        Leer argumentos de ejecución
    */
    parse_arguments(argc, argv, &parametros);

    /*
        Reservar memoria para ADN y patrones
    */
    char* cadena_adn = vector_alloc(parametros.dna_length);

    patron_t *patrones =reservar_patrones(parametros.cantidad_patrones, parametros.longitud_patron);
   
    generar_adn(cadena_adn, parametros.longitud_adn);

    generar_patrones(patrones, parametros.longitud_patron, parametros.cantidad_patrones);

    buscar_patrones_pthread(cadena_adn, parametros.longitud_adn, patrones, parametros.cantidad_patrones,parametros.num_threads);

    /*
        Imprimir resultados
    */
    for (int i = 0; i < parametros.k_patterns; i++) {

        printf(
            "Patron %d [%s] - Estado: [%d]",
            i,
            patrones[i].patron,
            patrones[i].estado
        );

        if (patrones[i].estado == ENCONTRADO) {

            printf(
                " - Posicion: %d\n",
                patrones[i].encontrado_en
            );

        } else {

            printf("\n");
        }
    }

    for (int i = 0; i < parametros.k_patterns; i++) {

        free(patternes[i].patron);
    }

    free(patrones);

    free(cadena_adn);

    return 0;
}