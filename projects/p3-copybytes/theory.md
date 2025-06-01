# Resolución de la práctica desglosada

## Descripción general

Este programa permite copiar bytes de un archivo origen a un archivo destino, haciendo uso de un buffer de tamaño variable especificado por el usuario. Adicionalmente, permite limitar el número total de bytes a copiar.

También admite el uso de `-` como nombre de archivo para usar la entrada o salida estándar (`stdin` o `stdout`).

---

## Estructura base

La ejecución del programa se realizará de la siguiente forma:

```bash
./copybytes <origen> <destino> <buffer> [bytes]
```

Donde:

* `<origen>`: archivo de entrada (usar `-` para `stdin`)
* `<destino>`: archivo de salida (usar `-` para `stdout`)
* `<buffer>`: tamaño del buffer para las operaciones de lectura y escritura
* `[bytes]`: opcional, número máximo de bytes a copiar

---

## Algoritmo de copiado

1. **Verificación de argumentos**
   Se validan el número y formato de los argumentos. Si son incorrectos, el programa finaliza informando del error.

2. **Apertura de archivos**
   Se abren los ficheros de origen y destino según los nombres introducidos:

   * Si el nombre es `-`, se usa `STDIN_FILENO` o `STDOUT_FILENO`.
   * Si se produce un error en la apertura, el programa aborta.

3. **Copia de bytes con buffer**
   Se aloja memoria dinámica (`malloc`) del tamaño indicado por el usuario.
   Se comienza un bucle de lectura y escritura:

   * Se lee del origen tantos bytes como permita el buffer.
   * Si hay un límite de bytes (`maxbytes`), se ajusta el último bloque leído.
   * Se escribe el bloque leído al archivo de destino.
   * Se suma la cantidad de bytes escritos.
   * Si se alcanza el máximo permitido, se interrumpe el bucle.

4. **Cierre de ficheros y liberación de recursos**
   Se cierran los descriptores y se libera el buffer reservado.

---

## Pseudocódigo de la solución

```c
main(argc, argv[]) {
    
    comprobacion de los argumentos;

    convertir buffer y (opcional) los bytes;

    abrir origen y destino (pueden ser stdin/stdout);

    reservar buffer;

    while (leer buffer desde origen > 0) {
        if límite de bytes -> ajustar bytes a copiar;

        escribir en destino;

        if límite alcanzado -> romper bucle;
    }

    cerrar fds y liberar memoria;
}
```

---

## Fases del desarrollo

### 1. Comprobación de argumentos

Se detecta si faltan parámetros o están mal formateados (p. ej., texto donde debería ir un número). Se usa `strtol` para parsear enteros con seguridad.

### 2. Apertura de archivos

El programa contempla los siguientes casos:

* `-` como nombre → entrada/salida estándar.
* Apertura normal con `open(...)`.
* Comprobación de errores con `perror`.

### 3. Bucle de lectura/escritura

El uso de un buffer dinámico permite copiar archivos grandes sin cargar todo en memoria. Además:

* Se usa un contador acumulado de bytes.
* Se comprueba si se alcanzó el límite impuesto por el usuario.

### 4. Liberación y cierre

La función `try_close` comprueba que no se cierre `stdin` ni `stdout` accidentalmente.

---

## Funciones destacadas

```c
int try_write(...) { ... }
```

Encargada del bucle de lectura y escritura. Controla el límite de bytes, errores de lectura/escritura y libera el buffer.

```c
void try_open(...) { ... }
```

Abre los ficheros según los parámetros pasados. Si se especifica `-`, usa `stdin` o `stdout`.

---