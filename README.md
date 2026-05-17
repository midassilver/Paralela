# Proyecto Cadenas de ADN

Programa en C para buscar patrones dentro de cadenas de ADN usando busqueda lineal (secuencial), hilos POSIX (pthreads) y paso de mensajes (MPI).

## Compilacion

Para compilar el programa:

"mpicc Proyecto/Secuencias_ADN/src/*.c -Iinclude -o programa"

## Uso

Para utilizar el programa:

- Ingresar: "cd Proyecto/Secuencias_ADN"

Elegir el modo en el cual ejecutar el programa:

- "make run": corre la vercion sin mpi.
- "make run_mpi": corre la version con mpi.
- "make test": se pueden hacer las pruebas al programa.

Para limpiar los archivos compilados "make clean"