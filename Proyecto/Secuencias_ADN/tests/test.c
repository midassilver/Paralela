#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/busqueda_MPI.h"
#include "../include/busqueda_lineal.h"
#include "../include/busqueda_opencl.h"
#include "../include/busqueda_pthread.h"

static patron_t crear_patron_literal(char* texto) {
    return (patron_t){
        .patron = texto,
        .longitud = (int)strlen(texto),
        .encontrado_en = NO_ENCONTRADO,
        .estado = EN_COLA
    };
}

static patron_t* copiar_patrones(char** textos, int cantidad) {
    patron_t* patrones = (patron_t*) malloc(sizeof(patron_t) * cantidad);

    assert(patrones != NULL);

    for (int i = 0; i < cantidad; i++) {
        int longitud = (int)strlen(textos[i]);

        patrones[i].patron = (char*) malloc((size_t)longitud + 1);
        assert(patrones[i].patron != NULL);

        strcpy(patrones[i].patron, textos[i]);
        patrones[i].longitud = longitud;
        patrones[i].encontrado_en = NO_ENCONTRADO;
        patrones[i].estado = EN_COLA;
    }

    return patrones;
}

static void liberar_patrones(patron_t* patrones, int cantidad) {
    if (patrones == NULL) {
        return;
    }

    for (int i = 0; i < cantidad; i++) {
        free(patrones[i].patron);
    }

    free(patrones);
}

static void comparar_resultados(patron_t* esperado, patron_t* obtenido, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        assert(esperado[i].encontrado_en == obtenido[i].encontrado_en);
        assert(esperado[i].estado == obtenido[i].estado);
    }
}

static void probar_casos_basicos_lineales(void) {
    struct {
        char* secuencia;
        char* patron;
        int posicion_esperada;
        int estado_esperado;
    } casos[] = {
        {"ATGCGT", "ATG", 0, COINCIDENCIA},
        {"ATGCGT", "CGT", 3, COINCIDENCIA},
        {"ATGCGT", "AAA", NO_ENCONTRADO, NO_ENCONTRADO},
        {"AAAAAA", "AAA", 0, COINCIDENCIA},
        {"ATGCGT", "ATGCGT", 0, COINCIDENCIA},
        {"ATG", "ATGC", NO_ENCONTRADO, NO_ENCONTRADO},
        {"CCCATGCCC", "ATG", 3, COINCIDENCIA}
    };

    int cantidad = (int)(sizeof(casos) / sizeof(casos[0]));

    for (int i = 0; i < cantidad; i++) {
        patron_t patron = crear_patron_literal(casos[i].patron);

        buscar_patrones_lineal(casos[i].secuencia, (int)strlen(casos[i].secuencia), &patron, 1);

        assert(patron.encontrado_en == casos[i].posicion_esperada);
        assert(patron.estado == casos[i].estado_esperado);
    }
}

static int probar_opencl_si_disponible(const char* adn, int longitud_adn, patron_t* esperado, char** textos, int cantidad) {
    patron_t* patrones_opencl = copiar_patrones(textos, cantidad);
    int disponible = buscar_patrones_opencl(adn, longitud_adn, patrones_opencl, cantidad) == 0;

    if (disponible) {
        comparar_resultados(esperado, patrones_opencl, cantidad);
    }

    liberar_patrones(patrones_opencl, cantidad);
    return disponible;
}

static void probar_versiones_con_casos_deterministicos(void) {
    const char* adn = "ATGCGTAAATTTCCCATGCGT";
    char* textos[] = {
        "ATG",
        "CGT",
        "AAA",
        "CCC",
        "GGG",
        "ATGCGT",
        "TCCCA",
        "ATGCGTA",
        "TTTCCC",
        "AAAA"
    };
    int cantidad = (int)(sizeof(textos) / sizeof(textos[0]));
    int longitud_adn = (int)strlen(adn);

    patron_t* patrones_lineal = copiar_patrones(textos, cantidad);
    patron_t* patrones_pthread = copiar_patrones(textos, cantidad);

    buscar_patrones_lineal(adn, longitud_adn, patrones_lineal, cantidad);
    buscar_patrones_pthread(adn, longitud_adn, patrones_pthread, cantidad, 3);
    destruir_hilos(3);

    comparar_resultados(patrones_lineal, patrones_pthread, cantidad);
    probar_opencl_si_disponible(adn, longitud_adn, patrones_lineal, textos, cantidad);

    liberar_patrones(patrones_lineal, cantidad);
    liberar_patrones(patrones_pthread, cantidad);
}

static void probar_versiones_con_carga_grande(void) {
    const int longitud_adn = 100000;
    const int cantidad_patrones = 50;
    const int longitud_patron = 5;

    char* adn = reservar_vector(longitud_adn);
    patron_t* patrones_base = reservar_patrones(cantidad_patrones, longitud_patron);

    srand(1234);
    generar_adn(adn, longitud_adn);
    generar_patrones(patrones_base, longitud_patron, cantidad_patrones);

    char** textos = (char**) malloc(sizeof(char*) * cantidad_patrones);
    assert(textos != NULL);

    for (int i = 0; i < cantidad_patrones; i++) {
        textos[i] = patrones_base[i].patron;
    }

    patron_t* patrones_lineal = copiar_patrones(textos, cantidad_patrones);
    patron_t* patrones_pthread = copiar_patrones(textos, cantidad_patrones);
    patron_t* patrones_mpi = copiar_patrones(textos, cantidad_patrones);

    buscar_patrones_lineal(adn, longitud_adn, patrones_lineal, cantidad_patrones);
    buscar_patrones_pthread(adn, longitud_adn, patrones_pthread, cantidad_patrones, 5);
    destruir_hilos(5);

    comparar_resultados(patrones_lineal, patrones_pthread, cantidad_patrones);
    probar_opencl_si_disponible(adn, longitud_adn, patrones_lineal, textos, cantidad_patrones);

    parametros_t mpi_parametros = {
        .cantidad_patrones = cantidad_patrones,
        .longitud_adn = longitud_adn,
        .longitud_patron = longitud_patron,
        .numero_hilos = 5
    };

    patrones_mpi = buscar_patrones_MPI(mpi_parametros, adn, patrones_mpi);
    comparar_resultados(patrones_lineal, patrones_mpi, cantidad_patrones);

    liberar_patrones(patrones_lineal, cantidad_patrones);
    liberar_patrones(patrones_pthread, cantidad_patrones);
    liberar_patrones(patrones_mpi, cantidad_patrones);

    for (int i = 0; i < cantidad_patrones; i++) {
        free(patrones_base[i].patron);
    }

    free(patrones_base);
    free(textos);
    free(adn);
}

int main(void) {
    probar_casos_basicos_lineales();
    probar_versiones_con_casos_deterministicos();
    probar_versiones_con_carga_grande();

    printf("Tests pasados\n");
    return 0;
}
