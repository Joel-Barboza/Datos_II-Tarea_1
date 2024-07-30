#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>

const uint64_t SMALL_SIZE = 512ULL * 1024 * 1024; // 512 MB
const uint64_t MEDIUM_SIZE = 1024ULL * 1024 * 1024; // 1 GB
const uint64_t LARGE_SIZE = 2ULL * 1024 * 1024 * 1024; // 2 GB

class BinaryFile {
public:

    BinaryFile(const std::string& Size, const std::string& Path) {
        uint64_t fileSize;
        if (Size == "SMALL") {
            fileSize = SMALL_SIZE;
        } else if (Size == "MEDIUM") {
            fileSize = MEDIUM_SIZE;
        } else if (Size == "LARGE") {
            fileSize = LARGE_SIZE;
        } else {
            std::cerr << "Tamaño no válido.\n";
            return;
        }

        std::ofstream outf(Path, std::ofstream::binary);
        if (!outf) {
            std::cerr << "No se pudo abrir el archivo para escribir.\n";
            return;
        }

        writeNInt(fileSize / sizeof(int), outf);
        outf.close();

        readBinaryFile();
    }

    void readBinaryFile() {
        /*std::ifstream inf(path, std::ifstream::binary);
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
        }*/
    }
private:
    static void writeNInt (uint64_t n, std::ofstream &outf) {
        srand(static_cast<unsigned>(time(nullptr)));
        std::cout << n << "\n";
        for (uint64_t i = 0; i < n; ++i) {
            int random = rand() % 10000;
            outf.write(reinterpret_cast<const char*>(&random), sizeof(random));
        }
    }
};




int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cout << "Faltan argumentos:\n";
        std::cout << "./generator -size <SMALL|MEDIUM|LARGE> -output <OUTPUT FILE PATH>\n";
        return 1;
    }

    std::string fileSize;
    std::string outputPath;

    // Starts in one cuz first elem in argv is always the name of the program (e.g. ./generator)
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-size") {
            if (std::string(argv[i+1]) == "SMALL" ||
                std::string(argv[i+1]) == "MEDIUM" ||
                std::string(argv[i+1]) == "LARGE") {
                fileSize = argv[i+1];
            } else {
                std::cout << "Tamaño no válido: -size <SMALL|MEDIUM|LARGE>\n";
                return 1;
            }
        } else if (std::string(argv[i]) == "-output") {
            outputPath = argv[i+1];
        }
    }

    if (fileSize.empty() || outputPath.empty()) {
        std::cerr << "Parámetros inválidos.\n";
        return 1;
    }

    BinaryFile file(fileSize, outputPath);
    return 0;
}
