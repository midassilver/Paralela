#include "../include/parametros.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

/* Valores predeterminados */
#define LONGITUD_ADN_DEFAULT       15000
#define CANTIDAD_PATRONES_DEFAULT  10
#define LONGITUD_PATRON_DEFAULT    5
#define NUMERO_HILOS_DEFAULT       4

void procesar_argumentos(int argc, char *argv[], parametros_t *parametros) {

    /* Inicializar valores por defecto */
    parametros->longitud_adn = LONGITUD_ADN_DEFAULT;
    parametros->cantidad_patrones = CANTIDAD_PATRONES_DEFAULT;
    parametros->longitud_patron = LONGITUD_PATRON_DEFAULT;
    parametros->numero_hilos = NUMERO_HILOS_DEFAULT;

    int opcion;

    /* Opciones largas */
    static struct option opciones_largas[] = {
        {"longitud-adn",       required_argument, 0, 'd'},
        {"cantidad-patrones", required_argument, 0, 'k'},
        {"longitud-patron",   required_argument, 0, 'p'},
        {"numero-hilos",      required_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    while ((opcion = getopt_long(argc, argv, "d:k:p:t:", opciones_largas, NULL)) != -1) {

        switch (opcion) {

            case 'd':
                parametros->longitud_adn = atoi(optarg);
                break;

            case 'k':
                parametros->cantidad_patrones = atoi(optarg);
                break;

            case 'p':
                parametros->longitud_patron = atoi(optarg);
                break;

            case 't':
                parametros->numero_hilos = atoi(optarg);
                break;

            default:
                fprintf(stderr,
                    "Uso:\n"
                    "  --longitud-adn <numero>\n"
                    "  --cantidad-patrones <numero>\n"
                    "  --longitud-patron <numero>\n"
                    "  --numero-hilos <numero>\n");
                exit(EXIT_FAILURE);
        }
    }
}