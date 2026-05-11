#ifndef BUSQUEDA_MPI_H
#define BUSQUEDA_MPI_H

#include "cadenas.h"
#include <mpi.h>

typedef struct {
    int encontrado_en;
    int estado;
} resultado_t;
/**
 * \brief Realiza una búsqueda por medio de MPI de todos los patrones en la cadena de ADN.
 *
 * \param cadena_adn Puntero a la cadena principal de ADN.
 * \param longitud_cadena_adn Longitud de la cadena principal de ADN.
 * \param patrones Arreglo de patrones a buscar.
 * \param cantidad_patrones Cantidad de patrones en el arreglo.
 */
void buscar_patrones_MPI(const char* cadena_adn,int longitud_cadena_adn,patron_t* patrones);

#endif