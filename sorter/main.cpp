#include <iostream>
#include <fstream>

// g++ -o sorter main.cpp
// ./sorter -input ../output.bin -output output.bin -alg BS

class PagedArray {

    std::string filePath;
    // indices de pagina inicializados en -1, para indicar que están vacios
    int frame1[128]{};
    int pageOnFrame1{-1};
    int frame2[128]{};
    int pageOnFrame2{-1};
    int frame3[128]{};
    int pageOnFrame3{-1};
    int frame4[128]{};
    int pageOnFrame4{-1};

    int oldestPageLoaded{-1};



    int defaultValue{45}; // Valor por defecto


    int &loadFromDisk(int pageNum) {
        /*if (pageIndexOnP1 == -1) {
            return page1[index % 128];
        } else if (pageIndexOnP2 == -1) {
            return page2[index % 128];
        } else if (pageIndexOnP3 == -1) {
            return page3[index % 128];
        } else if (pageIndexOnP4 == -1) {
            return page4[index % 128];
        } else {
            //loadFromDisk();
        }*/
        getNext128Int(128);
        return defaultValue;
    }

    void getNext128Int(int startIndex) {
        std::cout << "running\n";
        std::ifstream file(filePath, std::ifstream::binary);
        if (!file) {
            std::cerr << "No se pudo abrir el archivo para leer.\n";
            return;
        }

        int number;

        file.seekg(startIndex * sizeof(int), std::ifstream::beg);
        /*for (int i = 0; i < 128; ++i) {
            file.seekg(i * sizeof(int), std::ifstream::cur);*/

            //file.read(reinterpret_cast<char *>(&number), 128 * sizeof(int));
            //std::cout << number << "\n";
        /*}*/
    }

public:
    PagedArray(std::string resultFilePath) {
        filePath = {resultFilePath};
    }

    int &operator [](int index) {
        // Dividido por 128 porque cada pagina tiene esa cantidad de indices
        int pageNum{index / 128};

        if (pageOnFrame1 == pageNum) {
            return frame1[index % 128];
        } else if (pageOnFrame2 == pageNum) {
            return frame2[index % 128];
        } else if (pageOnFrame3 == pageNum) {
            return frame3[index % 128];
        } else if (pageOnFrame4 == pageNum) {
            return frame4[index % 128];
        } else {
            return loadFromDisk(pageNum);
        }
    }
};


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
    arr[3];


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
