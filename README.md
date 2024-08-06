# Aspectos generales
* Se usó g++ para la compilación de ambas partes de la tarea.
* Se usaron flags de optimización para la compilación.
#### Información de las flags usadas: 
* [GCC, Options That Control Optimization]("https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html")
* [-O3]("https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-O3")
* [-march=native]("https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html#index-march-16")
* [-flto]("https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-flto")
* [-funroll-loops]("https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-funroll-loops")

# Uso de generator

## Compilación
Abrir una terminal en la carpeta generator
```bash
g++ -O3 -march=native -flto -funroll-loops -o generator main.cpp
```


## Ejecución
Abrir una terminal en la carpeta generator
```bash
./generator –size <SIZE> -output <OUTPUT FILE PATH>
```
Donde **\<SIZE>** puede tomar 3 valores:
* SMALL: genera un archivo de 512 MB
* MEDIUM: genera un archivo de 1 GB
* LARGE: genera un archivo 2 GB

Mientras que **\<OUTPUT FILE PATH>** recibe la ruta en la que se creará el archivo resultante.

### Ejemplo de generación de archivo de 512MB
```bash
./generator –size SMALL -output ../output.bin
```

## Uso de sorter

## Compilación
Abrir una terminal en la carpeta generator
```bash
g++ -O3 -march=native -flto -funroll-loops -o sorter main.cpp
```


## Ejecución
Abrir una terminal en la carpeta generator
```bash
./sorter –input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg <ALGORITMO>
```
Donde **\<INPUT FILE PATH>** recibe la ruta del archivo que será copiado para realizar el ordenamiento.

Mientras que **\<OUTPUT FILE PATH>** recibe la ruta en la que se creará el archivo **ordenado** resultante.

Además, con el argumento **\<ALGORITMO>** se puede seleccionar entre 3 algoritmos de ordenamiento para dicho proceso:
* QS: Quick sort
* IS: Insertion sort
* BS: bubble sort


### Ejemplo de ordenamiento del archivo creado anteriormente por generator
```bash
./sorter -input ../output.bin -output output.bin -alg QS
```

