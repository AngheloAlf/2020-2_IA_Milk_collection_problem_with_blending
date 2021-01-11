# Milk Colleciton Problem with Blending

Estudiante: Anghelo Carvajal
Rol: 201473062-4

## Compilación

Para compilar el programa, se debe usar el `Makefile` incluido:

```bash
make
```

Lo anterior genera el ejecutable llamado `MCPwB.elf`.

Este `Makefile` también incluye las siguientes opciones:

- `make clean`: Borra los archivos de compilación y el ejecutable generado.
- `make tidy`: Usa un analizador estático en el código fuente y muestra los warnings por pantalla.
- `make perf`: Perfila las funciones del programa, ejecutándolo con todas las instancias disponibles.
- `make perf-graph`: Perfila las funciones del programa y presenta los resultados en forma de grafo.
- `make MODE=DEBUG`: Compila el ejecutable en modo debug.

## Ejecución

La sintaxis para ejecutar el programa es la siguiente:

```bash
./MCPwB.elf <ruta/instancia.txt> K
```

Donde `K` es la cantidad máxima de iteraciones a realizar.

### Ejemplos de ejecución

```bash
./MCPwB.elf instances/a36.txt 500
./MCPwB.elf instances/a76.txt 1000
```

### Debug print

Existe la posibilidad de imprimir por pantalla información extra durante la ejecución. Está pensada principalmente para ser usada durante el desarrollo.

Para poder ver esta información, se debe pasar un tercer parámetro al momento de ejecutar el programa, el cual debe ser 'true'.

Ejemplo:

```bash
./MCPwB.elf instances/a36.txt 1000 true
```

### Script para probar todas las instancias

Se puede usar el script `test_instances.sh` para probar el programa con todas las instancias disponibles en la carpeta `instances/`. Estas son ejecutadas de forma secuencial.

Este script recibe un parámetro, la cantidad de iteraciones máxima por instancia (`K`).

```bash
./test_instances.sh 1000
```

## Formato de salida

- Primera línea:
  - Calidad de la solución.
  - Costos totales del transporte.
  - Ganancia bruta según el tipo de leche.
- `N` líneas que muestran la información de las `N` rutas que fueron asignadas a los camiones (mostradas según el orden de los camiones):
  - Granjas asignadas a esta ruta, en el orden en el que se deben recorrer. También se muestra al inicio y al final el nodo correspondiente a la planta procesadora.
  - Costo de transporte de esta ruta.
  - Cantidad de leche transportada en esta ruta.
  - Calidad de la leche resultante.
- Finalmente, se muestra el tiempo total de la ejecución, medido en milisegundos.
