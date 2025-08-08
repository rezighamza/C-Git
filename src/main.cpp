#include "commands.hpp" 

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "c-git: fatal: not a c-git command. See 'c-git --help'." << std::endl;
        return 1;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    std::string command = args[0];

    if (command == "init") {
        handleInitCommand();
    } else if (command == "hash-object") {
        if (args.size() < 2) {
            std::cerr << "fatal: you must specify a file path." << std::endl;
            return 1;
        }
        handleHashObjectCommand(args[1]);
    } else if (command == "add") {
        if (args.size() < 2) {
            std::cerr << "fatal: you must specify a file path to add." << std::endl;
            return 1;
        }
        handleAddCommand(args[1]);
    } else if (command == "commit") {
        if (args.size() < 3 || (args[1] != "-m" && args[1] != "--message")) {
            std::cerr << "usage: c-git commit -m <message>" << std::endl;
            return 1;
        }
        handleCommitCommand(args[2]);
    } else {
        std::cerr << "c-git: '" << command << "' is not a c-git command." << std::endl;
        return 1;
    }

    return 0;
}