
#ifndef BUSQUEDA_PTHREAD_H
#define BUSQUEDA_PTHREAD_H

#include <pthread.h>
#include "cadenas.h"


#define NO_ENCONTRADO -1
#define ENCONTRADO 1


void* trabajador_hilo(void* arg);


void buscar_patrones_pthread(const char* cadena_adn, int longitud_adn, patron_t* patrones, int cantidad_patrones, int cantidad_hilos);

void destruir_hilos(int cantidad_hilos);

#endif