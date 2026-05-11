#ifndef BASE_H
#define BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/** \def COINCIDENCIA
 * \brief Estado que indica que el patrón fue encontrado.
 */
#define COINCIDENCIA 1

/** \def EN_COLA
 * \brief Estado que indica que el patrón está en espera de búsqueda.
 */
#define EN_COLA 0

/** \def NO_ENCONTRADO
 * \brief Estado que indica que el patrón no fue encontrado.
 */
#define NO_ENCONTRADO -1

/* Nucleótidos válidos del ADN */
extern const char NUCLEOTIDOS[];

/**
 * \struct patron_t
 * \brief Representa un patrón de ADN y su estado de búsqueda.
 */
typedef struct {
    char* patron;          /**< Cadena del patrón de ADN */
    int longitud;          /**< Longitud del patrón */
    int encontrado_en;     /**< Posición donde se encontró el patrón */
    int estado;            /**< Estado: NO_ENCONTRADO (-1), EN_COLA (0), COINCIDENCIA (1) */
} patron_t;

/**
 * \struct argumentos_hilo_t
 * \brief Argumentos enviados a hilos/procesos para ejecución paralela.
 */
typedef struct {
    const char* cadena_adn;      /**< Puntero a la cadena principal de ADN */
    int longitud_cadena_adn;     /**< Longitud de la cadena principal */
    patron_t* patrones;          /**< Arreglo de patrones a buscar */
    int indice_inicio;           /**< Índice inicial del trabajo */
    int indice_fin;              /**< Índice final del trabajo */
} argumentos_hilo_t;

/**
 * \brief Reserva memoria para un vector de caracteres.
 * \param n Tamaño del vector.
 * \return Puntero al arreglo de caracteres reservado.
 */
char* reservar_vector(int n);

/**
 * \brief Reserva memoria para un arreglo de patrones.
 * \param filas Cantidad de patrones.
 * \param columnas Longitud de cada patrón.
 * \return Puntero al arreglo de patron_t.
 */
patron_t* reservar_patrones(int filas, int columnas);

/**
 * \brief Genera una cadena de ADN aleatoria.
 * \param adn_ptr Puntero a la memoria reservada para el ADN.
 * \param n Longitud de la cadena de ADN.
 */
void generar_adn(char* adn_ptr, int n);

/**
 * \brief Genera patrones aleatorios para búsqueda.
 * \param patrones Arreglo de estructuras patron_t.
 * \param longitud Longitud de cada patrón.
 * \param cantidad_patrones Cantidad de patrones a generar.
 */
void generar_patrones(patron_t* patrones, int longitud, int cantidad_patrones);



#endif