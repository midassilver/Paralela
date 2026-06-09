#ifndef BUSQUEDA_OPENCL_H
#define BUSQUEDA_OPENCL_H

#include "cadenas.h"

/**
 * \brief Busca todos los patrones usando OpenCL.
 *
 * Ejecuta un kernel 2D donde cada work-item revisa una posicion de un patron.
 * Retorna 0 si la busqueda se ejecuto correctamente y -1 si OpenCL no pudo
 * inicializarse, compilarse o ejecutarse.
 */
int buscar_patrones_opencl(
    const char* cadena_adn,
    int longitud_adn,
    patron_t* patrones,
    int cantidad_patrones
);

#endif
