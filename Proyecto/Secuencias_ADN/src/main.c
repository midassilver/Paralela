#define _POSIX_C_SOURCE 199309L

#include "../include/cadenas.h"
#include "../include/busqueda_MPI.h"
#include "../include/busqueda_lineal.h"
#include "../include/busqueda_opencl.h"
#include "../include/busqueda_pthread.h"
#include "../include/parametros.h"

static double tiempo_actual_segundos(void) {
    struct timespec tiempo;
    clock_gettime(CLOCK_MONOTONIC, &tiempo);
    return (double)tiempo.tv_sec + (double)tiempo.tv_nsec / 1000000000.0;
}

static int contar_coincidencias(patron_t* patrones, int cantidad_patrones) {
    int encontrados = 0;

    for (int i = 0; i < cantidad_patrones; i++) {
        if (patrones[i].estado == COINCIDENCIA) {
            encontrados++;
        }
    }

    return encontrados;
}

static void imprimir_resultados(patron_t* patrones, int cantidad_patrones) {
    for (int i = 0; i < cantidad_patrones; i++) {

        printf("Patron %d [%s] - Estado [%d]", i, patrones[i].patron, patrones[i].estado);

        if (patrones[i].estado == COINCIDENCIA) {

            printf(" - Posicion %d\n", patrones[i].encontrado_en);

        } else {

            printf(" - No encontrado\n");
        }
    }
}

static void imprimir_resumen_tiempo(const char* nombre, double inicio, double fin, patron_t* patrones, int cantidad_patrones) {
    printf("Tiempo %s: %.6f segundos | Patrones encontrados: %d/%d\n",
        nombre,
        fin - inicio,
        contar_coincidencias(patrones, cantidad_patrones),
        cantidad_patrones
    );
}

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

    double inicio = tiempo_actual_segundos();
    buscar_patrones_lineal(cadena_adn,parametros.longitud_adn, patrones,parametros.cantidad_patrones);
    double fin = tiempo_actual_segundos();

    imprimir_resumen_tiempo("lineal", inicio, fin, patrones, parametros.cantidad_patrones);
    imprimir_resultados(patrones, parametros.cantidad_patrones);

    /*
        BUSQUEDA PTHREAD
    */
    printf("\n=== BUSQUEDA PTHREAD ===\n");

    inicio = tiempo_actual_segundos();
    buscar_patrones_pthread(cadena_adn, parametros.longitud_adn,patrones, parametros.cantidad_patrones,parametros.numero_hilos);
    fin = tiempo_actual_segundos();

    imprimir_resumen_tiempo("Pthreads", inicio, fin, patrones, parametros.cantidad_patrones);
    imprimir_resultados(patrones, parametros.cantidad_patrones);

    destruir_hilos(parametros.numero_hilos);

    /*
        BUSQUEDA OPENCL
    */
    printf("\n=== BUSQUEDA OPENCL ===\n");

    inicio = tiempo_actual_segundos();
    if (buscar_patrones_opencl(cadena_adn, parametros.longitud_adn, patrones, parametros.cantidad_patrones) == 0) {
        fin = tiempo_actual_segundos();

        imprimir_resumen_tiempo("OpenCL", inicio, fin, patrones, parametros.cantidad_patrones);
        imprimir_resultados(patrones, parametros.cantidad_patrones);

    } else {
        fin = tiempo_actual_segundos();

        printf("No se pudo ejecutar la busqueda OpenCL. Tiempo intento: %.6f segundos\n", fin - inicio);
    }

    /*
        BUSQUEDA MPI
    */
    printf("\n=== BUSQUEDA MPI ===\n");

    inicio = tiempo_actual_segundos();
    patron_t *patrones_MPI = buscar_patrones_MPI(parametros, cadena_adn, patrones);
    fin = tiempo_actual_segundos();

    imprimir_resumen_tiempo("MPI", inicio, fin, patrones_MPI, parametros.cantidad_patrones);
    imprimir_resultados(patrones_MPI, parametros.cantidad_patrones);

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
