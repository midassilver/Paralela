# Compilador
CC = mpicc

# Flags de compilación
CFLAGS = -Wall -Wextra -O2 -g -pthread
LDLIBS = -lOpenCL

# Flags extra para profiling (gprof)
PROF_FLAGS = -pg

# Carpetas
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
PROF_OBJ_DIR = obj_prof
PROF_BIN_DIR = bin_prof

# Nombre del ejecutable
TARGET = $(BIN_DIR)/programa
TARGET_PROF = $(PROF_BIN_DIR)/programa_prof

# Archivos fuente
SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/cadenas.c \
	$(SRC_DIR)/busqueda_lineal.c \
	$(SRC_DIR)/busqueda_MPI.c \
	$(SRC_DIR)/busqueda_opencl.c \
	$(SRC_DIR)/busqueda_pthread.c \
	$(SRC_DIR)/parametros.c

SRC := $(filter-out src/main.c, $(wildcard src/*.c))

# Archivos objeto
OBJECTS = $(SOURCES:.c=.o)

# Objetos de profiling (van a una carpeta aparte para no chocar con el build normal)
PROF_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(PROF_OBJ_DIR)/%.o,$(SOURCES))

# Regla principal
all: $(TARGET)

# Crear ejecutable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)

# Compilar archivos .c -> .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# --- Build de profiling (gprof) ---
# Compila todo con -pg además de las flags normales, en objetos separados
$(PROF_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(PROF_OBJ_DIR)
	$(CC) $(CFLAGS) $(PROF_FLAGS) -I$(INC_DIR) -c $< -o $@

$(TARGET_PROF): $(PROF_OBJECTS)
	@mkdir -p $(PROF_BIN_DIR)
	$(CC) $(CFLAGS) $(PROF_FLAGS) -o $(TARGET_PROF) $(PROF_OBJECTS) $(LDLIBS)

profiling: $(TARGET_PROF)
	@echo "Binario de profiling listo en $(TARGET_PROF)"
	@echo "Corre con: ./$(TARGET_PROF) -d <largo> -k <cant_patrones> -p <largo_patron> -t <hilos>"
	@echo "Luego: gprof $(TARGET_PROF) gmon.out > reporte_gprof.txt"

# Ejecutar programa
run: all
	./$(TARGET)

.PHONY: test

test:
	mpicc -g -pthread tests/test.c src/busqueda_lineal.c src/busqueda_MPI.c src/busqueda_opencl.c src/busqueda_pthread.c src/cadenas.c src/parametros.c -Iinclude -o test $(LDLIBS) && ./test

# Ejecutar con MPI
run_mpi: all
	mpirun -np 4 ./$(TARGET)

run_valgrind: all
	mpirun -np 4 valgrind --gen-suppressions=all ./$(TARGET)

# Limpiar archivos compilados
clean:
	rm -f $(SRC_DIR)/*.o
	rm -rf $(BIN_DIR)
	rm -rf $(PROF_OBJ_DIR) $(PROF_BIN_DIR)
	rm -f gmon.out

# Evitar conflictos con nombres de archivos
.PHONY: all run run_mpi clean profiling
