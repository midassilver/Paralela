__kernel void buscar_patrones_opencl(
    __global const char* secuencia,
    __global const char* patrones,
    __global const int* longitudes,
    __global int* resultados,
    const int longitud_secuencia,
    const int cantidad_patrones,
    const int stride_patron
) {
    int patron_id = get_global_id(0);
    int posicion = get_global_id(1);

    if (patron_id >= cantidad_patrones) {
        return;
    }

    int longitud_patron = longitudes[patron_id];

    if (posicion + longitud_patron > longitud_secuencia) {
        return;
    }

    for (int i = 0; i < longitud_patron; i++) {
        if (secuencia[posicion + i] != patrones[patron_id * stride_patron + i]) {
            return;
        }
    }

    atomic_min(&resultados[patron_id], posicion);
}
