#ifndef BUSQUEDA_LINEAL_H
#define BUSQUEDA_LINEAL_H

#include "cadenas.h"

/**
 * \brief Realiza una búsqueda lineal de todos los patrones en la cadena de ADN.
 *
 * \param cadena_adn Puntero a la cadena principal de ADN.
 * \param longitud_cadena_adn Longitud de la cadena principal de ADN.
 * \param patrones Arreglo de patrones a buscar.
 * \param cantidad_patrones Cantidad de patrones en el arreglo.
 */
void buscar_patrones_lineal(const char* cadena_adn,int longitud_cadena_adn,patron_t* patrones, int cantidad_patrones
);

#endif