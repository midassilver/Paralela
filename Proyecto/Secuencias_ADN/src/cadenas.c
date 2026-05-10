#include "../include/cadenas.h"

const char NUCLEOTIDOS[] = "ATCG";

char* reservar_vector(int n) {
    char *vector = (char *) malloc((n + 1) * sizeof(char));

    if (vector == NULL) {
        fprintf(stderr, "Error: No se pudo reservar memoria para el vector.\n");
        exit(1);
    }

    vector[n] = '\0';
    return vector;
}

patron_t* reservar_patrones(int filas, int columnas) {

    patron_t* patrones = (patron_t*) malloc(filas * sizeof(patron_t));

    if (patrones != NULL) {

        for (int i = 0; i < filas; i++) {

            patrones[i].patron = reservar_vector(columnas);
            patrones[i].encontrado_en = NO_ENCONTRADO;
            patrones[i].estado = EN_COLA;
        }

    } else {
        fprintf(stderr, "Error: No se pudo reservar memoria para el vector de patrones.\n");
    }

    return patrones;
}

void generar_adn(char* adn_ptr, int n) {

    for (int i = 0; i < n; i++) {
        adn_ptr[i] = NUCLEOTIDOS[rand() % 4];
    }

    adn_ptr[n] = '\0';
}

void generar_patrones(patron_t* patrones, int longitud, int cantidad_patrones) {

    for (int i = 0; i < cantidad_patrones; i++) {

        for (int j = 0; j < longitud; j++) {

            patrones[i].patron[j] = NUCLEOTIDOS[rand() % 4];
            patrones[i].longitud = longitud;
        }

        patrones[i].patron[longitud] = '\0';
        patrones[i].estado = EN_COLA;
    }
}