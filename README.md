# Aspectos generales
* Se usó g++ para la compilación de ambas partes de la tarea.
* Se usaron flags de optimización para la compilación.
* El comando generator crea los archivos según tamaño de manera correcta.
* Se agregó un valor de SIZE adicional a generator llamado **TEST**, ya que sorter dura demasiado con los archivos más grandes. Para un archivo de 128KB, Bubble Sort tarda 90 segundos, Insertion sort aproximadamente 40 segundos y Quick Sort ronda los 200 milisegundos. Quick Sort fue probado con archivos de 16MB, tardando un tiempo aproximado de 30 segundos.
* Inicialmente se usó **FIFO** (First In, First Out) para el remplazo de páginas cargadas, pero los pages faults fueron muy altos, por lo que luego se implemento **LRU** (Least Recently Used), que redujo los page faults en gran manera.
* LRU fue implementado de una manera muy simple, por lo que se averiguó sobre optimizaciones y se encontró que lo mas usado era la implementación por medio de un unorderen map (implementación en la rama [sorter/unordered_map](https://github.com/Joel-Barboza/Datos_II-Tarea_1/tree/sorter/unordered_map)) para consultas de páginas más rapidas, pero al implementarlo con PagedArray el tiempo más bien aumentó.

#### Información de las flags usadas: 
* [GCC, Options That Control Optimization](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)
* [-O3](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-O3)
* [-march=native](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html#index-march-16)
* [-flto](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-flto)
* [-funroll-loops](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-funroll-loops)

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
Donde **\<SIZE>** puede tomar 3 valores principales:
* SMALL: genera un archivo de 512 MB
* MEDIUM: genera un archivo de 1 GB
* LARGE: genera un archivo 2 GB

> [!IMPORTANT]
> Adicionalmente, se agregó el valor **TEST**, el cual genera un archivo de **64KB**, el cual es util para probar la funcionalidad de sorter, ya que archivos grandes toma un tiempo excesivo con algunos algoritmos

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

