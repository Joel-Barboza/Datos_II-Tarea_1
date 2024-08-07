#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>

//g++ -O3 -march=native -flto -funroll-loops -o generator main.cpp
//./generator -size SMALL -output ../output.bin

const unsigned long TEST_SIZE = 64 *1024; // 64KB
const unsigned long SMALL_SIZE = 512UL * 1024 * 1024; // 512 MB
const unsigned long MEDIUM_SIZE = 1024UL * 1024 * 1024; // 1 GB
const unsigned long LARGE_SIZE = 2UL * 1024 * 1024 * 1024; // 2 GB

class BinaryFile {
public:
    BinaryFile(const std::string &Size, const std::string &Path) {
        unsigned long fileSize;
        if (Size == "SMALL") {
            fileSize = SMALL_SIZE;
        } else if (Size == "MEDIUM") {
            fileSize = MEDIUM_SIZE;
        } else if (Size == "LARGE") {
            fileSize = LARGE_SIZE;
        } else if (Size == "TEST") {
            fileSize = TEST_SIZE;
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
        //writeOrderedInt(fileSize / sizeof(int), outf);
        outf.close();
    }

private:
    static void writeNInt(unsigned long n, std::ofstream &outf) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 99999);
        //std::cout << n << "\n";

        constexpr int bufferSize = 65536;
        std::vector<int> buffer;
        buffer.reserve(bufferSize);

        for (unsigned long i = 0; i < n; ++i) {
            int random = dis(gen);
            buffer.push_back(random);

            // escribe en archivo cuano el buffer se llena
            if (buffer.size() == bufferSize) {
                outf.write(reinterpret_cast<const char *>(buffer.data()), buffer.size() * sizeof(int));
                buffer.clear();
            }
        }
        // Escribe lo que haya quedado en el buffer
        if (!buffer.empty()) {
            outf.write(reinterpret_cast<const char *>(buffer.data()), buffer.size() * sizeof(int));
        }
    }

    void writeOrderedInt(unsigned long n, std::ofstream &outf) {
        //std::cout << n << "\n";

        constexpr int bufferSize = 65536;
        std::vector<int> buffer;
        buffer.reserve(bufferSize);


        for (long i = n - 1; i >= 0; --i) {
            //for (unsigned long i = 0; i < n; ++i) {
            buffer.push_back(i);

            // escribe en archivo cuano el buffer se llena
            if (buffer.size() == bufferSize) {
                outf.write(reinterpret_cast<const char *>(buffer.data()), buffer.size() * sizeof(int));
                buffer.clear();
            }
        }
        // Escribe lo que haya quedado en el buffer
        if (!buffer.empty()) {
            outf.write(reinterpret_cast<const char *>(buffer.data()), buffer.size() * sizeof(int));
        }
    }
};


int main(int argc, char *argv[]) {
    if (argc < 5) {
        std::cout << "Faltan argumentos:\n";
        std::cout << "./generator -size <SMALL|MEDIUM|LARGE|TEST> -output <OUTPUT FILE PATH>\n";
        return 1;
    }

    std::string fileSize;
    std::string outputPath;

    // Starts in one cuz first elem in argv is always the name of the program (e.g. ./generator)
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-size") {
            if (std::string(argv[i + 1]) == "SMALL" ||
                std::string(argv[i + 1]) == "MEDIUM" ||
                std::string(argv[i + 1]) == "LARGE" ||
                std::string(argv[i + 1]) == "TEST") {
                fileSize = argv[i + 1];
            } else {
                std::cout << "Tamaño no válido: -size <SMALL|MEDIUM|LARGE|TEST>\n";
                return 1;
            }
        } else if (std::string(argv[i]) == "-output") {
            outputPath = argv[i + 1];
        }
    }

    if (fileSize.empty() || outputPath.empty()) {
        std::cerr << "Parámetros inválidos\n";
        return 1;
    }

    BinaryFile file(fileSize, outputPath);
    std::cout << "Archivo creado, " << fileSize << std::endl;
    return 0;
}
