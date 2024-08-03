#include <iostream>
#include <fstream>

// g++ -O3 -o sorter main.cpp
// ./sorter -input ../output.bin -output output.bin -alg BS


class PagedArray {

    std::string filePath;
    // indices de pagina inicializados en -1, para indicar que están vacios
    int frame0[128]{};
    int pageOnFrame0{-1};
    int frame1[128]{};
    int pageOnFrame1{-1};
    int frame2[128]{};
    int pageOnFrame2{-1};
    int frame3[128]{};
    int pageOnFrame3{-1};

    int oldestFrameLoaded{-1};

    int totalSize;


    int defaultValue{-1}; // Valor por defecto


    int &loadFromDisk(int pageNum, int index) {
        if (pageOnFrame0 == -1) {
            loadToSelectedFrame(pageNum*128, frame0);
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            pageOnFrame0 = pageNum;
            return frame0[index % 128];

        } else if (pageOnFrame1 == -1) {
            loadToSelectedFrame(pageNum*128, frame1);
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            pageOnFrame1 = pageNum;
            return frame1[index % 128];
        } else if (pageOnFrame2 == -1) {
            loadToSelectedFrame(pageNum*128, frame2);
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            pageOnFrame2 = pageNum;
            return frame2[index % 128];
        } else if (pageOnFrame3 == -1) {
            loadToSelectedFrame(pageNum*128, frame3);
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            pageOnFrame3 = pageNum;
            return frame3[index % 128];
        } else {

            std::cout << oldestFrameLoaded <<" "<< pageOnFrame3 <<" "<<frame3[0] << "\n";
            std::cout << "before overwrite"<<"\n";
            for (int i = 0; i < 128; ++i) {
                std::cout << i << ": " << frame0[i] << ": " << frame1[i] << ": " << frame2[i] << ": " << frame3[i] << "\n";
            }
            return overwriteFrame(pageNum, index);
        }


    }

    void loadToSelectedFrame(int startIndex, int frame[128]) {
        std::ifstream file(filePath, std::ifstream::binary);
        if (!file) {
            std::cerr << "No se pudo abrir el archivo para leer.\n";
            return;
        }

        // Move to the start index of the page
        file.seekg(startIndex * sizeof(int), std::ifstream::beg);
        if (!file) {
            std::cerr << "Error al posicionar el archivo.\n";
            return;
        }

        // Read 128 integers into the frame
        file.read(reinterpret_cast<char *>(frame), 128 * sizeof(int));
        if (!file) {
            std::cerr << "Error al leer datos del archivo.\n";
        }
        std::cout << "load to from disk"<<"\n";
        for (int i = 0; i < 128; ++i) {
            std::cout << i << ": " << frame0[i] << ": " << frame1[i] << ": " << frame2[i] << ": " << frame3[i] << "\n";
        }

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

    int& overwriteFrame(int pageNum, int index) {
        if (oldestFrameLoaded == 0) {
            downloadFrame(pageOnFrame0, frame0);
            std::cout << "load from overwrite"<<"\n";
            loadToSelectedFrame(pageNum * 128, frame0);

            pageOnFrame0 = pageNum;
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            std::cout << oldestFrameLoaded <<" "<< pageOnFrame0 <<" "<<frame0[0]<< "\n";
            std::cout << "OVERWRITE"<<"\n";
            for (int i = 0; i < 128; ++i) {
                std::cout << i << ": " << frame0[i] << ": " << frame1[i] << ": " << frame2[i] << ": " << frame3[i] << "\n";
            }
            frame0[127] = 2;
            std::cout << frame0[127];
            std::cout << 127 << ": " << frame0[127] << ": " << frame1[127] << ": " << frame2[127] << ": " << frame3[127] << "\n";
            //std::exit(0);
            return frame0[index % 128];
        } else if (oldestFrameLoaded == 1) {
            downloadFrame(pageOnFrame1, frame1);
            loadToSelectedFrame(pageNum * 128, frame1);
            pageOnFrame1 = pageNum;
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            return frame1[index % 128];
        } else if (oldestFrameLoaded == 2) {
            downloadFrame(pageOnFrame2, frame2);
            loadToSelectedFrame(pageNum * 128, frame2);
            pageOnFrame2 = pageNum;
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            return frame2[index % 128];
        } else if (oldestFrameLoaded == 3) {
            downloadFrame(pageOnFrame3, frame3);
            loadToSelectedFrame(pageNum * 128, frame3);
            pageOnFrame3 = pageNum;
            oldestFrameLoaded = ++oldestFrameLoaded % 4;
            return frame3[index % 128];
        }
        std::cout << "No se pudo sobreescribir el frame" << std::endl;
        //oldestFrameLoaded = ++oldestFrameLoaded % 4;
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
        if (pageOnFrame0 == pageNum) {
            return frame0[index % 128];
        } else if (pageOnFrame1 == pageNum) {
            return frame1[index % 128];
        } else if (pageOnFrame2 == pageNum) {
            return frame2[index % 128];
        } else if (pageOnFrame3 == pageNum) {
            return frame3[index % 128];
        } else {
            return loadFromDisk(pageNum, index);
        }
    }
    int size() const {
        return totalSize;
    }
    void writeAsCsv (std::string& binFile, std::string csvFile) {
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
        while (src.read(reinterpret_cast<char*>(&number), sizeof(int))) {
            dst << number << ",";
        }

        // Remove the last comma
        dst.seekp(-1, std::ios_base::end);
        dst << '\n';

        std::cout << "Archivo convertido exitosamente a CSV.\n";
    }


};



void swap(int &a, int &b) {
    int temp = a;
    a = b;
    b = temp;
}
void bubbleSort(PagedArray &arr, int n)
{
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

int partition(PagedArray &arr, int start, int end)
{

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

void quickSort(PagedArray &arr, int start, int end)
{

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



int main(int argc, char *argv[]) {
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
            if (std::string(argv[i + 1]) == "QS" ||
                std::string(argv[i + 1]) == "IS" ||
                std::string(argv[i + 1]) == "BS") {
                algorithm = argv[i + 1];
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


    std::ifstream src(inputPath, std::ifstream::binary);
    if (!src) {
        std::cerr << "No se pudo abrir el archivo para leer.\n";
        return 1;
    }
    std::ofstream dst(outputPath, std::ofstream::binary);
    if (!dst) {
        std::cerr << "No se pudo abrir el archivo para escribir.\n";
        return 1;
    }
    dst << src.rdbuf();

    PagedArray arr{outputPath};
    int N = arr.size();
    std::cout << N << "\n";
    // Debug: Print the first 10 values before sorting
    std::cout << "Array before sorting:\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
    //bubbleSort(arr, N);
    quickSort(arr, 0, N-1);
    arr.writeAsCsv(outputPath, "output.txt");
    /*int j;
    for (j = 0; j < N; j++)
        std::cout << arr[j] << " ";
    std::cout << std::endl;*/
    /*std::cout << arr[3] << "\n";*/
    // Debug: Print the first 10 values after sorting
    std::cout << "Array after sorting:\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";

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
