#include "../include/busqueda_opencl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 120
#endif

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

static char* leer_archivo_kernel(const char* ruta, size_t* tamano) {
    FILE* archivo = fopen(ruta, "rb");

    if (archivo == NULL) {
        return NULL;
    }

    if (fseek(archivo, 0, SEEK_END) != 0) {
        fclose(archivo);
        return NULL;
    }

    long longitud = ftell(archivo);

    if (longitud < 0) {
        fclose(archivo);
        return NULL;
    }

    rewind(archivo);

    char* contenido = (char*) malloc((size_t) longitud + 1);

    if (contenido == NULL) {
        fclose(archivo);
        return NULL;
    }

    size_t leidos = fread(contenido, 1, (size_t) longitud, archivo);
    fclose(archivo);

    contenido[leidos] = '\0';
    *tamano = leidos;

    return contenido;
}

static char* leer_kernel_opencl(size_t* tamano) {
    const char* rutas[] = {
        "src/busqueda_opencl.cl",
        "../src/busqueda_opencl.cl",
        "busqueda_opencl.cl"
    };

    for (size_t i = 0; i < sizeof(rutas) / sizeof(rutas[0]); i++) {
        char* codigo = leer_archivo_kernel(rutas[i], tamano);

        if (codigo != NULL) {
            return codigo;
        }
    }

    return NULL;
}

static cl_device_id obtener_dispositivo(cl_platform_id* plataforma_out) {
    cl_uint cantidad_plataformas = 0;
    cl_int error = clGetPlatformIDs(0, NULL, &cantidad_plataformas);

    if (error != CL_SUCCESS || cantidad_plataformas == 0) {
        return NULL;
    }

    cl_platform_id* plataformas = (cl_platform_id*) malloc(sizeof(cl_platform_id) * cantidad_plataformas);

    if (plataformas == NULL) {
        return NULL;
    }

    error = clGetPlatformIDs(cantidad_plataformas, plataformas, NULL);

    if (error != CL_SUCCESS) {
        free(plataformas);
        return NULL;
    }

    cl_device_id dispositivo = NULL;

    for (cl_uint i = 0; i < cantidad_plataformas && dispositivo == NULL; i++) {
        error = clGetDeviceIDs(plataformas[i], CL_DEVICE_TYPE_GPU, 1, &dispositivo, NULL);

        if (error == CL_SUCCESS) {
            *plataforma_out = plataformas[i];
            break;
        }
    }

    for (cl_uint i = 0; i < cantidad_plataformas && dispositivo == NULL; i++) {
        error = clGetDeviceIDs(plataformas[i], CL_DEVICE_TYPE_CPU, 1, &dispositivo, NULL);

        if (error == CL_SUCCESS) {
            *plataforma_out = plataformas[i];
            break;
        }
    }

    free(plataformas);
    return dispositivo;
}

static int preparar_patrones(
    patron_t* patrones,
    int cantidad_patrones,
    char** patrones_planos,
    int** longitudes,
    int* stride_patron
) {
    int max_longitud = 0;

    for (int i = 0; i < cantidad_patrones; i++) {
        if (patrones[i].longitud > max_longitud) {
            max_longitud = patrones[i].longitud;
        }
    }

    if (max_longitud <= 0) {
        return -1;
    }

    char* planos = (char*) calloc((size_t)cantidad_patrones * (size_t)max_longitud, sizeof(char));
    int* largos = (int*) malloc(sizeof(int) * cantidad_patrones);

    if (planos == NULL || largos == NULL) {
        free(planos);
        free(largos);
        return -1;
    }

    for (int i = 0; i < cantidad_patrones; i++) {
        largos[i] = patrones[i].longitud;
        memcpy(&planos[i * max_longitud], patrones[i].patron, (size_t)patrones[i].longitud);
    }

    *patrones_planos = planos;
    *longitudes = largos;
    *stride_patron = max_longitud;

    return 0;
}

static void aplicar_resultados(patron_t* patrones, int cantidad_patrones, int* resultados, int sin_resultado) {
    for (int i = 0; i < cantidad_patrones; i++) {
        if (resultados[i] == sin_resultado) {
            patrones[i].encontrado_en = NO_ENCONTRADO;
            patrones[i].estado = NO_ENCONTRADO;
        } else {
            patrones[i].encontrado_en = resultados[i];
            patrones[i].estado = COINCIDENCIA;
        }
    }
}

int buscar_patrones_opencl(
    const char* cadena_adn,
    int longitud_adn,
    patron_t* patrones,
    int cantidad_patrones
) {
    if (cadena_adn == NULL || patrones == NULL || longitud_adn <= 0 || cantidad_patrones <= 0) {
        fprintf(stderr, "Error OpenCL: parametros invalidos.\n");
        return -1;
    }

    char* patrones_planos = NULL;
    int* longitudes = NULL;
    int stride_patron = 0;

    if (preparar_patrones(patrones, cantidad_patrones, &patrones_planos, &longitudes, &stride_patron) != 0) {
        fprintf(stderr, "Error OpenCL: no se pudieron preparar los patrones.\n");
        return -1;
    }

    int sin_resultado = longitud_adn;
    int* resultados = (int*) malloc(sizeof(int) * cantidad_patrones);

    if (resultados == NULL) {
        free(patrones_planos);
        free(longitudes);
        fprintf(stderr, "Error OpenCL: no se pudo reservar memoria para resultados.\n");
        return -1;
    }

    for (int i = 0; i < cantidad_patrones; i++) {
        resultados[i] = sin_resultado;
    }

    size_t tamano_kernel = 0;
    char* codigo_kernel = leer_kernel_opencl(&tamano_kernel);

    if (codigo_kernel == NULL) {
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        fprintf(stderr, "Error OpenCL: no se pudo leer el archivo busqueda_opencl.cl.\n");
        return -1;
    }

    cl_platform_id plataforma = NULL;
    cl_device_id dispositivo = obtener_dispositivo(&plataforma);

    if (dispositivo == NULL) {
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se encontro dispositivo GPU ni CPU compatible.\n");
        return -1;
    }

    cl_int error = CL_SUCCESS;
    cl_context contexto = clCreateContext(NULL, 1, &dispositivo, NULL, NULL, &error);

    if (error != CL_SUCCESS) {
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudo crear el contexto (%d).\n", error);
        return -1;
    }

    cl_command_queue cola = clCreateCommandQueue(contexto, dispositivo, 0, &error);

    if (error != CL_SUCCESS) {
        clReleaseContext(contexto);
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudo crear la cola de comandos (%d).\n", error);
        return -1;
    }

    const char* fuentes[] = { codigo_kernel };
    cl_program programa = clCreateProgramWithSource(contexto, 1, fuentes, &tamano_kernel, &error);

    if (error != CL_SUCCESS) {
        clReleaseCommandQueue(cola);
        clReleaseContext(contexto);
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudo crear el programa (%d).\n", error);
        return -1;
    }

    error = clBuildProgram(programa, 1, &dispositivo, NULL, NULL, NULL);

    if (error != CL_SUCCESS) {
        size_t log_tamano = 0;
        clGetProgramBuildInfo(programa, dispositivo, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_tamano);

        if (log_tamano > 0) {
            char* log = (char*) malloc(log_tamano + 1);

            if (log != NULL) {
                clGetProgramBuildInfo(programa, dispositivo, CL_PROGRAM_BUILD_LOG, log_tamano, log, NULL);
                log[log_tamano] = '\0';
                fprintf(stderr, "Log de compilacion OpenCL:\n%s\n", log);
                free(log);
            }
        }

        clReleaseProgram(programa);
        clReleaseCommandQueue(cola);
        clReleaseContext(contexto);
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudo compilar el kernel (%d).\n", error);
        return -1;
    }

    cl_kernel kernel = clCreateKernel(programa, "buscar_patrones_opencl", &error);

    if (error != CL_SUCCESS) {
        clReleaseProgram(programa);
        clReleaseCommandQueue(cola);
        clReleaseContext(contexto);
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudo crear el kernel (%d).\n", error);
        return -1;
    }

    size_t tamano_adn = sizeof(char) * (size_t)longitud_adn;
    size_t tamano_patrones = sizeof(char) * (size_t)cantidad_patrones * (size_t)stride_patron;
    size_t tamano_longitudes = sizeof(int) * (size_t)cantidad_patrones;
    size_t tamano_resultados = sizeof(int) * (size_t)cantidad_patrones;

    cl_mem buffer_adn = clCreateBuffer(contexto, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, tamano_adn, (void*)cadena_adn, &error);
    cl_mem buffer_patrones = clCreateBuffer(contexto, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, tamano_patrones, patrones_planos, &error);
    cl_mem buffer_longitudes = clCreateBuffer(contexto, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, tamano_longitudes, longitudes, &error);
    cl_mem buffer_resultados = clCreateBuffer(contexto, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, tamano_resultados, resultados, &error);

    if (buffer_adn == NULL || buffer_patrones == NULL || buffer_longitudes == NULL || buffer_resultados == NULL || error != CL_SUCCESS) {
        if (buffer_adn != NULL) clReleaseMemObject(buffer_adn);
        if (buffer_patrones != NULL) clReleaseMemObject(buffer_patrones);
        if (buffer_longitudes != NULL) clReleaseMemObject(buffer_longitudes);
        if (buffer_resultados != NULL) clReleaseMemObject(buffer_resultados);
        clReleaseKernel(kernel);
        clReleaseProgram(programa);
        clReleaseCommandQueue(cola);
        clReleaseContext(contexto);
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudieron crear los buffers (%d).\n", error);
        return -1;
    }

    error  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_adn);
    error |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_patrones);
    error |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_longitudes);
    error |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &buffer_resultados);
    error |= clSetKernelArg(kernel, 4, sizeof(int), &longitud_adn);
    error |= clSetKernelArg(kernel, 5, sizeof(int), &cantidad_patrones);
    error |= clSetKernelArg(kernel, 6, sizeof(int), &stride_patron);

    if (error != CL_SUCCESS) {
        clReleaseMemObject(buffer_adn);
        clReleaseMemObject(buffer_patrones);
        clReleaseMemObject(buffer_longitudes);
        clReleaseMemObject(buffer_resultados);
        clReleaseKernel(kernel);
        clReleaseProgram(programa);
        clReleaseCommandQueue(cola);
        clReleaseContext(contexto);
        free(patrones_planos);
        free(longitudes);
        free(resultados);
        free(codigo_kernel);
        fprintf(stderr, "Error OpenCL: no se pudieron configurar argumentos (%d).\n", error);
        return -1;
    }

    size_t trabajo_global[2] = {
        (size_t)cantidad_patrones,
        (size_t)longitud_adn
    };

    error = clEnqueueNDRangeKernel(cola, kernel, 2, NULL, trabajo_global, NULL, 0, NULL, NULL);

    if (error == CL_SUCCESS) {
        error = clFinish(cola);
    }

    if (error == CL_SUCCESS) {
        error = clEnqueueReadBuffer(cola, buffer_resultados, CL_TRUE, 0, tamano_resultados, resultados, 0, NULL, NULL);
    }

    if (error == CL_SUCCESS) {
        aplicar_resultados(patrones, cantidad_patrones, resultados, sin_resultado);
    } else {
        fprintf(stderr, "Error OpenCL: fallo la ejecucion o lectura del kernel (%d).\n", error);
    }

    clReleaseMemObject(buffer_adn);
    clReleaseMemObject(buffer_patrones);
    clReleaseMemObject(buffer_longitudes);
    clReleaseMemObject(buffer_resultados);
    clReleaseKernel(kernel);
    clReleaseProgram(programa);
    clReleaseCommandQueue(cola);
    clReleaseContext(contexto);
    free(patrones_planos);
    free(longitudes);
    free(resultados);
    free(codigo_kernel);

    return error == CL_SUCCESS ? 0 : -1;
}
