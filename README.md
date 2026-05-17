# Proyecto Cadenas de ADN

Programa en C para buscar patrones dentro de cadenas de ADN usando busqueda lineal (secuencial), hilos POSIX (pthreads) y paso de mensajes (MPI).

## Estructura del proyecto

|---- PARALELA
|  |---- README.md
|  |---- Proyecto
|  |  |---- Secuencias_ADN
|  |  |  |---- makefile
|  |  |  |---- include
|  |  |  |  |---- busqueda_lineal.h
|  |  |  |  |---- busqueda_MPI.h
|  |  |  |  |---- busqueda_pthread.h
|  |  |  |  |---- cadenas.h
|  |  |  |  |---- parametros.h
|  |  |  |---- src
|  |  |  |  |---- busqueda_lineal.c
|  |  |  |  |---- busqueda_MPI.c
|  |  |  |  |---- busqueda_pthread.c
|  |  |  |  |---- cadenas.c
|  |  |  |  |---- parametros.c
|  |  |  |  |---- main.c
|  |  |  |---- tests
|  |  |  |  |---- test.c

## Compilacion

Para compilar el programa:

"mpicc Proyecto/Secuencias_ADN/src/*.c -Iinclude -o programa"

## Uso

### Para utilizar el programa:

Ejecutar con valores por defecto:

mpirun -np 4 ./bin/program

Para ejecutar con parametros personalizados:

mpirun -np 4 ./bin/program -d "cantidad" -k "cantidad" -p "cantidad" -t "cantidad"

Donde:
- -d es la longitud de la cadena.
- -k es el numero de patrones a buscar.
- -p es la longitud del patron.
- -t es la cantidad de hilos.

### Tambien se puede utilizar por medio de:

- Ingresar: "cd Proyecto/Secuencias_ADN"

Elegir el modo en el cual ejecutar el programa:

- "make run": corre la vercion sin mpi.
- "make run_mpi": corre la version con mpi.
- "make test": se pueden hacer las pruebas al programa.

Para limpiar los archivos compilados "make clean"