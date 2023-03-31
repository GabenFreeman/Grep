#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int files = 0;
int nfiles = 0;
int patterns = 0;

void search(const std::string& pattern, const fs::path& path) {
    if (!fs::is_directory(path)) {
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(path)) {

        if (!entry.is_regular_file()) {
            continue;
        }

        std::ifstream file(entry.path());
        if (!file) {
            files++;
            continue;
        }

        std::string line;
        bool temp = true;
        while (std::getline(file, line)) {
            if (line.find(pattern) != std::string::npos) {
                std::cout << entry.path() << ": " << line << '\n';
                patterns++;
                if (temp){
                    nfiles++;
                    temp = false;
                }
            }
        }
        files++;
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <pattern> <directory>\n";
        return 1;
    }

    const std::string pattern = argv[1];
    const fs::path path = argv[2];

    search(pattern, path);
    std::cout << "Searched files: " << files << '\n';
    std::cout << "Files with pattern: " << nfiles << '\n';
    std::cout << "Patterns number: " << patterns << '\n';
    return 0;
}