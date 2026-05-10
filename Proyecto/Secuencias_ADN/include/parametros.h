#ifndef PARAMETROS_H
#define PARAMETROS_H

/**
 * @struct parametros_t
 * @brief Centraliza todos los parámetros de configuración para la búsqueda de patrones de ADN.
 */
typedef struct {
    int longitud_adn;
    int cantidad_patrones;
    int longitud_patron;
    int numero_hilos;
} parametros_t;

/**
 * \brief Procesa los argumentos de línea de comandos y llena la estructura de parámetros.
 *
 * Inicializa los parámetros con valores predeterminados. Luego procesa
 * los argumentos usando `getopt_long` para sobrescribir los valores
 * especificados por el usuario.
 *
 * \param argc Cantidad de argumentos recibidos por el programa.
 * \param argv Arreglo de cadenas con los argumentos de línea de comandos.
 * \param parametros Puntero a la estructura parametros_t que será llenada
 * con la configuración procesada.
 */
void procesar_argumentos(int argc, char *argv[], parametros_t *parametros);

#endif