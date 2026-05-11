#include "../include/cadenas.h"
#include "../include/busqueda_MPI.h"
#include "../include/parametros.h"



patron_t* buscar_patrones_MPI(parametros_t parametros) { /*TODO: Actualmente solo puede usarse con los valores aleatorios generados por 
                                                      generar_adn y generar_patrones */
    MPI_Init(NULL, NULL); //Inicializa el proceso de mpi

    int rango, tamaño; //Nos da el rango de cada proceso y la cantidad de procesos

    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &tamaño);

    char *cadena_adn_MPI = reservar_vector(parametros.longitud_adn);
    patron_t *patrones_MPI = reservar_patrones(parametros.cantidad_patrones, parametros.longitud_patron);

    if(rango == 0){ //Codigo unico del root que genera los datos
        generar_adn(cadena_adn_MPI, parametros.longitud_adn);
        generar_patrones(patrones_MPI, parametros.longitud_patron, parametros.cantidad_patrones);
    }
    //Brodcast de todos los datos a todos los procesos
    MPI_Bcast(cadena_adn_MPI, parametros.longitud_adn + 1, MPI_CHAR, 0, MPI_COMM_WORLD); //Esto envia la cadena de adn a los demas procesos
    for(int i = 0; i < parametros.cantidad_patrones; i++){
        MPI_Bcast(&patrones_MPI[i].longitud, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(&patrones_MPI[i].encontrado_en, 1, MPI_INT, 0, MPI_COMM_WORLD);

        MPI_Bcast(&patrones_MPI[i].estado, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(patrones_MPI[i].patron, patrones_MPI[i].longitud + 1, MPI_CHAR, 0, MPI_COMM_WORLD);
    }

    //Aqui calculamos que patrones_MPI le tocan a cada proceso
    int patrones_MPI_por_proceso = parametros.cantidad_patrones / tamaño;
    int residuo = parametros.cantidad_patrones % tamaño;
    int inicio = rango * patrones_MPI_por_proceso;
    int fin = inicio + patrones_MPI_por_proceso;
    if(rango == tamaño - 1){
        fin += residuo;
    }
    int cantidad_local = fin - inicio;
    resultado_t *locales = malloc((cantidad_local) * sizeof(resultado_t));

    //Revisamos los patrones_MPI
    for(int i = inicio; i < fin; i++){
        busqueda_por_patron(cadena_adn_MPI, parametros.longitud_adn, &patrones_MPI[i]);
        int indice_local = i - inicio;
        locales[indice_local].encontrado_en = patrones_MPI[i].encontrado_en;
        locales[indice_local].estado =  patrones_MPI[i].estado;
    }
    //Datos para el gather de el proceso root
    int *recvcounts = NULL; // cantidad de datos que manda cada proceso
    int *displs = NULL; //desplazamento de los procesos
    resultado_t *globales = NULL;

    if(rango == 0){
        recvcounts = malloc(tamaño * sizeof(int));
        displs = malloc(tamaño * sizeof(int));
    }

    //Gather de cuantos patrones_MPI cada proceso reviso
    MPI_Gather(&cantidad_local, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Este if del root prepara los datos necesarios para gatherv
    if(rango == 0){
        displs[0] = 0;
        recvcounts[0] *= sizeof(resultado_t);
        for(int i = 1; i < tamaño; i++){
            recvcounts[i] *= sizeof(resultado_t);
            displs[i] = displs[i - 1] + recvcounts[i - 1];
        }
        globales = malloc(parametros.cantidad_patrones * sizeof(resultado_t));
    }
    //Obtiene la informacion de todos los procesos
    MPI_Gatherv(locales, cantidad_local * sizeof(resultado_t), MPI_BYTE, globales, recvcounts, displs, MPI_BYTE, 0, MPI_COMM_WORLD);

    //Rejunta toda la informacion
    if(rango == 0){
        for(int i = 0; i < parametros.cantidad_patrones; i++){
            patrones_MPI[i].encontrado_en = globales[i].encontrado_en;
            patrones_MPI[i].estado = globales[i].estado;
        }
    }

    MPI_Finalize();

    free(locales);

    if(rango == 0){
        free(recvcounts);
        free(displs);
        free(globales);
    }

    return patrones_MPI;
}

void busqueda_por_patron(const char* cadena_adn, int longitud_cadena_adn, patron_t* patrones) {
    char* patron = patrones->patron;
    int longitud_patron = patrones->longitud;

    int coincidencia_en = -1;

    int fin_busqueda = longitud_cadena_adn - longitud_patron;

    for (int i = 0; i <= fin_busqueda; i++) {

        int j;

        for (j = 0; j < longitud_patron; j++) {

            if (cadena_adn[i + j] != patron[j]) {
                break;
            }
        }

        if (j == longitud_patron) {

            coincidencia_en = i;
            break;
        }
    }

    if (coincidencia_en != -1) {

        patrones->encontrado_en = coincidencia_en;
        patrones->estado = COINCIDENCIA;

    } else {

        patrones->estado = NO_ENCONTRADO;
    }
}
