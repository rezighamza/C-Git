#include "commands.hpp"
#include "objects.hpp"
#include "utils.hpp"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <ctime>

namespace fs = std::filesystem;

void handleInitCommand() {
    try {
        fs::create_directory(".cgit");
        fs::create_directory(".cgit/objects");
        fs::create_directory(".cgit/refs");
        std::ofstream head_file(".cgit/HEAD");
        if (head_file.is_open()) {
            head_file << "ref: refs/heads/main\n";
            head_file.close();
            std::cout << "Initialized empty C-Git repository in " << fs::current_path() / ".cgit/" << std::endl;
        } else {
            std::cerr << "fatal: unable to create .cgit/HEAD file" << std::endl;
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "fatal: " << e.what() << std::endl;
    }
}

void handleHashObjectCommand(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "fatal: could not open file " << filePath << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    try {
        std::string hash = writeBlob(buffer.str());
        std::cout << hash << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error during hash-object: " << e.what() << std::endl;
    }
}

void handleAddCommand(const std::string& filePath) {
    if (!fs::exists(filePath)) {
        std::cerr << "fatal: pathspec '" << filePath << "' did not match any files" << std::endl;
        return;
    }

    std::ifstream file(filePath, std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    std::string hash = writeBlob(content);

    fs::path indexPath = ".cgit/index";
    std::map<std::string, std::string> index_entries;

    if (fs::exists(indexPath)) {
        std::ifstream index_in(indexPath);
        std::string line;
        while (std::getline(index_in, line)) {
            std::stringstream line_stream(line);
            std::string mode, entry_hash, entry_path;
            line_stream >> mode >> entry_hash >> entry_path;
            index_entries[entry_path] = entry_hash;
        }
    }

    // Add or update the entry for our file
    index_entries[filePath] = hash;

    // Write the entire index back to the file
    std::ofstream index_out(indexPath);
    for (const auto& pair : index_entries) {
        // We hardcode the mode "100644" for simplicity
        index_out << "100644 " << pair.second << " " << pair.first << "\n";
    }
    
    std::cout << "Added '" << filePath << "' to the index." << std::endl;
}

void handleCommitCommand(const std::string& message) {
    try {
        std::string root_tree_hash = buildTreeFromIndex();

        std::string parent_hash;
        fs::path head_path = ".cgit/HEAD";
        if (fs::exists(head_path)) {
            std::ifstream head_file(head_path);
            std::string head_content;
            std::getline(head_file, head_content);
            std::string ref_path_str = head_content.substr(5); 
            fs::path ref_path = fs::path(".cgit") / ref_path_str;

            if (fs::exists(ref_path)) {
                std::ifstream parent_hash_file(ref_path);
                std::getline(parent_hash_file, parent_hash);
            }
        }

        std::string author = "Your Name <you@example.com>";
        long long timestamp = time(NULL); 
        
        std::stringstream commit_content;
        commit_content << "tree " << root_tree_hash << "\n";
        if (!parent_hash.empty()) {
            commit_content << "parent " << parent_hash << "\n";
        }
        commit_content << "author " << author << " " << timestamp << " +0000\n";
        commit_content << "committer " << author << " " << timestamp << " +0000\n\n";
        commit_content << message << "\n";
        
        std::string commit_str = commit_content.str();
        std::string commit_header = "commit " + std::to_string(commit_str.length()) + '\0';
        std::string full_commit_object = commit_header + commit_str;

        std::string commit_hash = calculate_sha1(full_commit_object);
        std::string compressed_content = compress_string(full_commit_object);

        std::string dir_name = commit_hash.substr(0, 2);
        std::string file_name = commit_hash.substr(2);
        fs::path object_dir = fs::path(".cgit") / "objects" / dir_name;
        fs::create_directories(object_dir);
        fs::path object_path = object_dir / file_name;
        std::ofstream object_file(object_path, std::ios::binary);
        object_file.write(compressed_content.c_str(), compressed_content.length());

        std::ifstream head_file(head_path);
        std::string head_content;
        std::getline(head_file, head_content);
        std::string ref_path_str = head_content.substr(5);
        fs::path ref_path = fs::path(".cgit") / ref_path_str;
        fs::create_directories(ref_path.parent_path()); // Ensure refs/heads exists
        std::ofstream ref_file(ref_path);
        ref_file << commit_hash << "\n";

        std::cout << "[" << ref_path_str << " " << commit_hash.substr(0, 7) << "] " << message << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Error during commit: " << e.what() << std::endl;
    }
}
