#include <chrono>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>

// g++ -O3 -march=native -flto -funroll-loops -o sorter main.cpp
// ./sorter -input ../output.bin -output output.bin -alg BS


class PagedArray {
public:
    PagedArray(std::string resultFilePath) {
        filePath = {resultFilePath};
        totalSize = calculateTotalSize();
        if (totalSize == -1) {
            std::cerr << "Error al calcular el tamaño del archivo.\n";
        }
    }

    int &operator [](int index) {
        if (index < 0 || index >= totalSize) {
            std::cerr << "Index out of bounds\n";
            return defaultValue;
        }

        // Dividido por 128 porque cada pagina tiene esa cantidad de indices
        int pageNum{index / 128};

        // if page loaded on frame, get element, else load it from disk
        if (pageNumbers[0] == pageNum) {
            pageHits++;
            return frames[0][index % 128];
        } else if (pageNumbers[1] == pageNum) {
            pageHits++;
            return frames[1][index % 128];
        } else if (pageNumbers[2] == pageNum) {
            pageHits++;
            return frames[2][index % 128];
        } else if (pageNumbers[3] == pageNum) {
            pageHits++;
            return frames[3][index % 128];
        } else {
            pageFaults++;
            return loadFromDisk(pageNum, index);
        }
    }

    int size() {
        return totalSize;
    }

    void downloadLastEditedFrames() {
        for (int i = 0; i < 4; ++i) {
            downloadFrame(pageNumbers[i], frames[i]);
        }
    }

    void writeAsCsv(std::string &binFile, std::string csvFile) {
        std::cout << "Copiando a csv.";
        std::ifstream src(binFile, std::ifstream::binary);
        if (!src) {
            std::cerr << "No se pudo abrir el archivo para leer.\n";
            return;
        }

        std::ofstream dst(csvFile);
        if (!dst) {
            std::cerr << "No se pudo abrir el archivo para escribir.\n";
            return;
        }

        int number;
        while (src.read(reinterpret_cast<char *>(&number), sizeof(int))) {
            dst << number << ",";
        }

        // Remove the last comma
        dst.seekp(-1, std::ios_base::end);
        dst << '\n';

        std::cout << "Archivo copiado exitosamente a CSV.\n";
    }

    unsigned long long getPageFaults() {
        return pageFaults;
    }

    unsigned long long getPageHits() {
        return pageHits;
    }

private:
    std::string filePath;
    // indices de pagina inicializados en -1, para indicar que están vacios
    const std::vector<int *> frames = {new int[128], new int[128], new int[128], new int[128]};
    std::vector<int> pageNumbers = {-1, -1, -1, -1};

    // LRU = Least Recently Used
    std::list<int> LRUOrder;

    int totalSize;


    int defaultValue{-1}; // Valor por defecto

    unsigned long long pageFaults = 0;
    unsigned long long pageHits = 0;


    int &loadFromDisk(int pageNum, int index) {
        if (pageNumbers[0] == -1) {
            loadToSelectedFrame(pageNum, 0);
            return frames[0][index % 128];
        } else if (pageNumbers[1] == -1) {
            loadToSelectedFrame(pageNum, 1);
            return frames[1][index % 128];
        } else if (pageNumbers[2] == -1) {
            loadToSelectedFrame(pageNum, 2);
            return frames[2][index % 128];
        } else if (pageNumbers[3] == -1) {
            loadToSelectedFrame(pageNum, 3);
            return frames[3][index % 128];
        } else {
            return overwriteFrame(pageNum, index);
        }
    }

    void loadToSelectedFrame(int pageNum, int frameIndex) {
        std::ifstream file(filePath, std::ifstream::binary);
        if (!file) {
            std::cerr << "No se pudo abrir el archivo para leer.\n";
            return;
        }

        // Move to the start index of the page
        file.seekg(pageNum * 128 * sizeof(int), std::ifstream::beg);
        if (!file) {
            std::cerr << "Error al posicionar el archivo.\n";
            return;
        }

        // Read 128 integers into the frame
        file.read(reinterpret_cast<char *>(frames[frameIndex]), 128 * sizeof(int));
        if (!file) {
            std::cerr << "Error al leer datos del archivo.\n";
        }
        LRUOrder.push_front(frameIndex);
        pageNumbers[frameIndex] = pageNum;
    }

    void downloadFrame(int pageOnFrame, int frame[128]) {
        std::ofstream file(filePath, std::ofstream::binary | std::ofstream::in | std::ofstream::out);
        if (!file) {
            std::cerr << "No se pudo abrir el archivo para escribir.\n";
            return;
        }

        file.seekp(pageOnFrame * 128 * sizeof(int), std::ofstream::beg);
        file.write(reinterpret_cast<const char *>(frame), 128 * sizeof(int));
    }

    int &overwriteFrame(int pageNum, int indexSearched) {
        int LRUFrameIndex = LRUOrder.back();
        downloadFrame(pageNumbers[LRUFrameIndex], frames[LRUFrameIndex]);
        LRUOrder.pop_back();
        loadToSelectedFrame(pageNum, LRUFrameIndex);
        return frames[LRUFrameIndex][indexSearched % 128];
    }

    int calculateTotalSize() {
        std::ifstream file(filePath, std::ifstream::binary | std::ifstream::ate);
        if (!file) {
            std::cerr << "No se pudo abrir el archivo para leer.\n";
            return -1;
        }

        std::streamsize fileSize = file.tellg();
        std::cout << "File size in bytes: " << fileSize << std::endl;
        return static_cast<int>(fileSize / sizeof(int));
    }
};


void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}

void bubbleSort(PagedArray &arr, int n) {
    int i, j;
    for (i = 0; i < n - 1; i++)

        // Last i elements are already
        // in place
        for (j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1]) {
                //std::cout << arr[j] << " " << arr[j+1] << "\n";
                swap(arr[j], arr[j + 1]);
            }
}

int partition(PagedArray &arr, int start, int end) {
    int pivot = arr[start];

    int count = 0;
    for (int i = start + 1; i <= end; i++) {
        if (arr[i] <= pivot)
            count++;
    }

    // Giving pivot element its correct position
    int pivotIndex = start + count;
    swap(arr[pivotIndex], arr[start]);

    // Sorting left and right parts of the pivot element
    int i = start, j = end;

    while (i < pivotIndex && j > pivotIndex) {
        while (arr[i] <= pivot) {
            i++;
        }

        while (arr[j] > pivot) {
            j--;
        }

        if (i < pivotIndex && j > pivotIndex) {
            swap(arr[i++], arr[j--]);
        }
    }

    return pivotIndex;
}

void quickSort(PagedArray &arr, int start, int end) {
    // base case
    if (start >= end)
        return;

    // partitioning the array
    int p = partition(arr, start, end);

    // Sorting the left part
    quickSort(arr, start, p - 1);

    // Sorting the right part
    quickSort(arr, p + 1, end);
}

void copyFile(const std::string &srcPath, const std::string &dstPath) {
    std::ifstream src(srcPath, std::ifstream::binary); // Open file in binary mode
    if (!src) {
        std::cerr << "No se pudo abrir el archivo fuente para leer.\n";
        return;
    }

    std::ofstream dst(dstPath, std::ofstream::binary); // Open file in binary mode
    if (!dst) {
        std::cerr << "No se pudo abrir el archivo de destino para escribir.\n";
        return;
    }

    dst << src.rdbuf(); // Copy contents of src to dst

    std::cout << "Archivo copiado exitosamente de " << srcPath << " a " << dstPath << ".\n";
}


int main(int argc, char *argv[]) {
    auto initialTime = std::chrono::high_resolution_clock::now();

    if (argc < 7) {
        std::cout << "Faltan argumentos:\n";
        std::cout << "./sorter –input <INPUT FILE PATH> -output <OUTPUT FILE PATH> -alg <ALGORITMO>\n";
        return 1;
    }

    std::string algorithm;
    std::string inputPath;
    std::string outputPath;

    // Starts in one cuz first elem in argv is always the name of the program (e.g. ./sorter)
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-input") {
            inputPath = argv[i + 1];
        } else if (std::string(argv[i]) == "-output") {
            outputPath = argv[i + 1];
        } else if (std::string(argv[i]) == "-alg") {
            if (std::string(argv[i + 1]) == "QS") {
                algorithm = "Quick Sort";
            } else if (std::string(argv[i + 1]) == "IS") {
                algorithm = "Insertion Sort";
            } else if (std::string(argv[i + 1]) == "BS") {
                algorithm = "Bubble Sort";
            } else {
                std::cout << "Tamaño no válido: -size <QS|IS|BS>\n";
                return 1;
            }

        }
    }

    if (algorithm.empty() || inputPath.empty() || outputPath.empty()) {
        std::cerr << "Parámetros inválidos\n";
        return 1;
    }


    copyFile(inputPath, outputPath);

    PagedArray arr{outputPath};
    int N = arr.size();
    quickSort(arr, 0, N - 1);
    arr.downloadLastEditedFrames();
    arr.writeAsCsv(outputPath, "output.txt");


    auto finishTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> miliseconds = initialTime - finishTime;

    unsigned long long pageFaults = arr.getPageFaults();
    unsigned long long pageHits = arr.getPageHits();

    //std::cout << "Tiempo durado: " << miliseconds << "ms\n";
    std::cout << "Algoritmo utilizado: " << algorithm << "\n";
    std::cout << "Page faults: " << pageFaults << "\n";
    std::cout << "Page hits: " << pageHits << "\n";

    return 0;
}


/*void readBinaryFile() {
    std::ifstream inf(path, std::ifstream::binary);
    if (!inf) {
        std::cerr << "No se pudo abrir el archivo para leer.\n";
        return;
    }

    int number;
    std::cout << "Leído del archivo:\n";
        std::cout << sizeof(number) << "\n";
    int i{0};
    for (int i = 0; i < 100; ++i) {
        inf.seekg(i * sizeof(int), std::ifstream::beg);
        inf.read(reinterpret_cast<char*>(&number), sizeof(number));
        std::cout << "Número " << i << ": " << number << "\n";
    }
}*/
