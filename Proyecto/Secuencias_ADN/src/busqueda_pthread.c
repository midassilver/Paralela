#include "../include/busqueda_pthread.h"
#include "../include/parametros.h"
#include "../include/cadenas.h"

static pthread_t* hilos = NULL;
static argumentos_hilo_t* argumentos = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t hay_trabajo = PTHREAD_COND_INITIALIZER;
static pthread_cond_t trabajo_terminado = PTHREAD_COND_INITIALIZER;
static int siguiente_patron = 0;
static int trabajos_pendientes = 0;
static int activo = 1;
static int hilos_creados = 0;

void* trabajador_hilo(void* arg) {

    argumentos_hilo_t* datos = (argumentos_hilo_t*) arg;

    while (1) { //Cambiamos a while porque no hay indices fijos

        pthread_mutex_lock(datos->mutex); //Solo un hilo a la vez lee o modifica

        while(*datos->siguiente_patron >= datos->cantidad_patrones && *datos->activo){
            pthread_cond_wait(datos->hay_trabajo, datos->mutex);
        }

        if(!*datos->activo){
            pthread_mutex_unlock(datos->mutex);
            break;
        }

        int patron_actual = *datos->siguiente_patron;
        (*datos->siguiente_patron)++;
        //Guardamos y actualizamos el patron un hilo a la vez

        pthread_mutex_unlock(datos->mutex);

        patron_t* patron_estructura = &datos->patrones[patron_actual];

        char* patron = patron_estructura->patron;

        int longitud_patron = patron_estructura->longitud;

        int coincidencia_en = NO_ENCONTRADO;

        int fin_busqueda = datos->longitud_cadena_adn - longitud_patron;

        for (int i = 0; i <= fin_busqueda; i++) {

            int j;

            for (j = 0; j < longitud_patron; j++) {

                if (datos->cadena_adn[i + j] != patron[j]) {
                    break;
                }
            }

            if (j == longitud_patron) {
                coincidencia_en = i;
                break;
            }
        }

        if (coincidencia_en != NO_ENCONTRADO) {

            patron_estructura->encontrado_en = coincidencia_en;
            patron_estructura->estado = COINCIDENCIA;

        } else {

            patron_estructura->encontrado_en = NO_ENCONTRADO;
            patron_estructura->estado = NO_ENCONTRADO;
        }

        pthread_mutex_lock(datos->mutex);
        (*datos->trabajos_pendientes)--;

        if(*datos->trabajos_pendientes == 0){
            pthread_cond_signal(datos->trabajo_terminado);
        }

        pthread_mutex_unlock(datos->mutex);
    }

    return NULL;
}

void buscar_patrones_pthread(
    const char* cadena_adn,
    int longitud_adn,
    patron_t* patrones,
    int cantidad_patrones,
    int cantidad_hilos
) {

    if (cantidad_hilos <= 0) {
        fprintf(stderr, "Error: la cantidad de hilos debe ser mayor que cero.\n");
        return;
    }

    //Los hilos se crean solo una vez y las variables se mantienen entre llamadas

    if(hilos == NULL){
        hilos = malloc(sizeof(pthread_t) * cantidad_hilos);
        argumentos = malloc(sizeof(argumentos_hilo_t) * cantidad_hilos);

        if (hilos == NULL || argumentos == NULL) {
            fprintf(stderr, "Error: no se pudo reservar memoria para los hilos.\n");
            free(hilos);
            free(argumentos);
            hilos = NULL;
            argumentos = NULL;
            return;
        }

        hilos_creados = cantidad_hilos;
        activo = 1;

        siguiente_patron = 0;

        for (int i = 0; i < cantidad_hilos; i++) {

            argumentos[i].cadena_adn = cadena_adn;

            argumentos[i].longitud_cadena_adn = longitud_adn;

            argumentos[i].patrones = patrones;

            argumentos[i].cantidad_patrones = 0;

            argumentos[i].siguiente_patron = &siguiente_patron;

            argumentos[i].mutex = &mutex;

            argumentos[i].hay_trabajo         = &hay_trabajo;

            argumentos[i].trabajo_terminado   = &trabajo_terminado;

            argumentos[i].activo              = &activo;

            argumentos[i].trabajos_pendientes = &trabajos_pendientes;


            if (pthread_create(
                &hilos[i],
                NULL,
                trabajador_hilo,
                &argumentos[i]
            ) != 0) {
                fprintf(stderr, "Error: no se pudo crear el hilo %d.\n", i);

                pthread_mutex_lock(&mutex);
                activo = 0;
                pthread_cond_broadcast(&hay_trabajo);
                pthread_mutex_unlock(&mutex);

                for (int j = 0; j < i; j++) {
                    pthread_join(hilos[j], NULL);
                }

                free(hilos);
                free(argumentos);
                hilos = NULL;
                argumentos = NULL;
                hilos_creados = 0;
                activo = 1;
                return;
            }
        }
    } else if (cantidad_hilos != hilos_creados) {
        fprintf(stderr, "Error: el pool de hilos ya fue creado con %d hilos.\n", hilos_creados);
        return;
    }
    
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < cantidad_hilos; i++) {

        argumentos[i].cadena_adn = cadena_adn;

        argumentos[i].longitud_cadena_adn = longitud_adn;

        argumentos[i].patrones = patrones;

        argumentos[i].cantidad_patrones = cantidad_patrones;
    }

    siguiente_patron = 0;
    trabajos_pendientes = cantidad_patrones;

    pthread_cond_broadcast(&hay_trabajo);

    while(trabajos_pendientes > 0){
        pthread_cond_wait(&trabajo_terminado, &mutex);
    }

    pthread_mutex_unlock(&mutex);
    
}

void destruir_hilos(int cantidad_hilos){
    if (hilos == NULL) {
        return;
    }

    (void)cantidad_hilos;
    int total_hilos = hilos_creados;

    pthread_mutex_lock(&mutex);

    activo = 0;

    pthread_cond_broadcast(&hay_trabajo);

    pthread_mutex_unlock(&mutex);

    for(int i = 0; i < total_hilos; i++){
        pthread_join(hilos[i], NULL);
    }

    free(hilos);
    free(argumentos);

    hilos = NULL;
    argumentos = NULL;
    hilos_creados = 0;
    activo = 1;
}
