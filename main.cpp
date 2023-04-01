#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <string.h>
#include <typeinfo>
#include <unistd.h>
#include <chrono>


namespace fs = std::filesystem;

int search(int argc, char** argv) {
    int files = 0;
    int nfiles = 0;
    int patterns = 0;

    //set default Current working directory
    char dir[256];
    getcwd(dir, 256);
    fs::path path = dir;

    //set pattern
    const std::string pattern = argv[1];

    //set default result file
    std::string txt=".txt";
    std::string resiltFileName = argv[0]+txt;
    resiltFileName.erase(0, 2);

    //set default log file
    std::string log=".log";
    std::string logFileName = argv[0]+log;
    logFileName.erase(0, 2);

    // Start measuring time
    auto begin = std::chrono::high_resolution_clock::now();

    for (int i = 2; i < argc; i+=2){
        //std::cout << argv[i] << std::endl ;
        //std::cout << i << std::endl ;

        if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dir")){
            path = argv[i+1];
            std::cout << "-d" <<": " << argv[i+1] <<std::endl;
        }
        else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log_file")){
            logFileName=argv[i+1]+log;
            std::cout << "-l" <<": " << argv[i+1]<< std::endl;
        }
        else if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--result_file")){
            resiltFileName=argv[i+1]+txt;
            std::cout << "-r" <<": " << argv[i+1] <<std::endl;
        }
        else if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--threads")){
            std::cout << "-t" <<": " << argv[i+1] <<std::endl;
        }
        else{
            std::cerr << "error\n";
            return 1;
        }

    }

    //create resiltFile
    std::ofstream resiltFile(resiltFileName);

    //create logFile
    std::ofstream logFile(logFileName);

    if (!fs::is_directory(path)) {
        return 1;
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
        int coutnLine = 0;
        while (std::getline(file, line)) {
            coutnLine++;
            if (line.find(pattern) != std::string::npos) {
                resiltFile << entry.path() << ":" << coutnLine << ": " << line << '\n';
                patterns++;
                if (temp){
                    nfiles++;
                    temp = false;
                }
            }
        }
        files++;
    }

    // Stop measuring time and calculate the elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    std::cout << "Searched files: " << files << '\n';
    std::cout << "Files with pattern: " << nfiles << '\n';
    std::cout << "Patterns number: " << patterns << '\n';
    std::cout << "Result file: " << resiltFileName << '\n';
    std::cout << "Log file: " << logFileName << '\n';
    printf("Time measured: %.6f seconds.\n", elapsed.count() * 1e-9);
    resiltFile.close();
    logFile.close();
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <pattern> <directory>\n";
        return 1;
    }

    //const std::string pattern = argv[1];
    //const fs::path path = argv[2];

    search(argc, argv);
    return 0;
}