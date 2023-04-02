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

	//check flags
    for (int i = 2; i < argc; i+=2){
        if(!strcmp(argv[i], "-d") || !strcmp(argv[i], "--dir")){
            path = argv[i+1];
        }
        else if(!strcmp(argv[i], "-l") || !strcmp(argv[i], "--log_file")){
            logFileName=argv[i+1]+log;
        }
        else if(!strcmp(argv[i], "-r") || !strcmp(argv[i], "--result_file")){
            resiltFileName=argv[i+1]+txt;
        }
        else if(!strcmp(argv[i], "-t") || !strcmp(argv[i], "--threads")){
            //not in this program :(
        }
        else{
            std::cerr << "Wrong flag.\n";
            return 1;
        }
    }

    //create resiltFile
    std::ofstream resiltFile(resiltFileName);

    //create logFile
    std::ofstream logFile(logFileName);

	//search for pattern
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
        std::cerr << "Usage: " << argv[0] << " <pattern> <flags>\n"
        "The program has one obligatory parameter which is <pattern (a string)>\n"
        "and can have four optional parameters.\n"
        "This program searches for a specified pattern in files within a specified\n"
        "directory and its subdirectories, and outputs the matches to a result file.\n\n"
        "+----------------------+--------------------------------+--------------------+\n"
        "|       Parameter      |          Description           |   Default value    |\n"
        "+----------------------+--------------------------------+--------------------+\n"
        "| -d or --dir          | start directory where program  |                    |\n"
        "|                      | needs to look for files        | current directory  |\n"
        "|                      | (also in subfolders)           |                    |\n"
        "+----------------------+--------------------------------+--------------------+\n"
        "| -l or --log_file     |     name of the log file       | <program name>.log |\n"
        "+----------------------+--------------------------------+--------------------+\n"
        "| -r or --result_file  |     name of the file where     | <program name>.txt |\n"
        "|                      |        result is given         |                    |\n"
        "+----------------------+--------------------------------+--------------------+\n"
        "| -t or --threads      |  number of threads in the pool |         4          |\n"
        "|                      |    (not in this program :( )   |                    |\n"
        "+----------------------+--------------------------------+--------------------+\n";
        return 1;
    }

    search(argc, argv);
    return 0;
}