
#include "../include/cadenas.h"
#include "../include/busqueda_MPI.h"
#include "../include/busqueda_lineal.h"
#include "../include/busqueda_opencl.h"
#include "../include/busqueda_pthread.h"
#include "../include/parametros.h"

int main(int argc, char* argv[]) {

    parametros_t parametros;

    procesar_argumentos(argc, argv, &parametros);

    char *cadena_adn =reservar_vector(parametros.longitud_adn);

    patron_t *patrones = reservar_patrones(parametros.cantidad_patrones, parametros.longitud_patron);

    generar_adn(cadena_adn,parametros.longitud_adn);

    generar_patrones( patrones, parametros.longitud_patron,parametros.cantidad_patrones);

    /*
        BUSQUEDA LINEAL
    */
    printf("\n=== BUSQUEDA LINEAL ===\n");

    buscar_patrones_lineal(cadena_adn,parametros.longitud_adn, patrones,parametros.cantidad_patrones);

    for (int i = 0; i < parametros.cantidad_patrones; i++) {

        printf("Patron %d [%s] - Estado [%d]", i, patrones[i].patron, patrones[i].estado);

        if (patrones[i].estado == COINCIDENCIA) {

            printf( " - Posicion %d\n", patrones[i].encontrado_en);

        } else {

            printf(" - No encontrado\n");
        }
    }

    /*
        BUSQUEDA PTHREAD
    */
    printf("\n=== BUSQUEDA PTHREAD ===\n");

    buscar_patrones_pthread(cadena_adn, parametros.longitud_adn,patrones, parametros.cantidad_patrones,parametros.numero_hilos);

    for (int i = 0; i < parametros.cantidad_patrones; i++) {

        printf("Patron %d [%s] - Estado [%d]", i, patrones[i].patron, patrones[i].estado );

        if (patrones[i].estado == COINCIDENCIA) {

            printf(" - Posicion %d\n", patrones[i].encontrado_en );

        } else {

            printf(" - No encontrado\n");
        }
    }

    destruir_hilos(parametros.numero_hilos);

    /*
        BUSQUEDA OPENCL
    */
    printf("\n=== BUSQUEDA OPENCL ===\n");

    if (buscar_patrones_opencl(cadena_adn, parametros.longitud_adn, patrones, parametros.cantidad_patrones) == 0) {

        for (int i = 0; i < parametros.cantidad_patrones; i++) {

            printf("Patron %d [%s] - Estado [%d]", i, patrones[i].patron, patrones[i].estado);

            if (patrones[i].estado == COINCIDENCIA) {

                printf(" - Posicion %d\n", patrones[i].encontrado_en);

            } else {

                printf(" - No encontrado\n");
            }
        }

    } else {

        printf("No se pudo ejecutar la busqueda OpenCL.\n");
    }

    /*
        BUSQUEDA MPI
    */
    printf("\n=== BUSQUEDA MPI ===\n");

    patron_t *patrones_MPI = buscar_patrones_MPI(parametros, cadena_adn, patrones);

    for (int i = 0; i < parametros.cantidad_patrones; i++) {

        printf("Patron %d [%s] - Estado [%d]",
            i, patrones_MPI[i].patron, patrones_MPI[i].estado);

        if (patrones_MPI[i].estado == COINCIDENCIA) {

            printf(" - Posicion %d\n",patrones_MPI[i].encontrado_en);

        } else {

            printf(" - No encontrado\n");
        }
    }

    /*
        Liberar memoria
    */
    for (int i = 0; i < parametros.cantidad_patrones; i++) {

        free(patrones[i].patron);
    }

    free(patrones);

    free(cadena_adn);

    return 0;
}
