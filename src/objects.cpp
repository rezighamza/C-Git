#include "objects.hpp"
#include "utils.hpp" // We need our utility functions

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace fs = std::filesystem;

std::string writeBlob(const std::string& content) {
    std::string object_str = "blob " + std::to_string(content.length()) + '\0' + content;

    std::string sha1_hash = calculate_sha1(object_str);

    std::string compressed_content = compress_string(object_str);

    std::string dir_name = sha1_hash.substr(0, 2);
    std::string file_name = sha1_hash.substr(2);
    fs::path object_dir = fs::path(".cgit") / "objects" / dir_name;
    fs::create_directories(object_dir); // Use create_directories for safety
    
    fs::path object_path = object_dir / file_name;
    std::ofstream object_file(object_path, std::ios::binary);
    object_file.write(compressed_content.c_str(), compressed_content.length());

    return sha1_hash;
}

std::string buildTree(const std::map<std::string, std::string>& index) {
    std::vector<IndexEntry> tree_entries;
    std::map<std::string, std::map<std::string, std::string>> subtrees;

    for (const auto& pair : index) {
        fs::path p(pair.first);
        if (p.has_parent_path() && !p.parent_path().empty()) {
            subtrees[p.parent_path().string()][p.filename().string()] = pair.second;
        } else {
            tree_entries.push_back({"100644", pair.second, p.string()});
        }
    }

    for (const auto& pair : subtrees) {
        std::map<std::string, std::string> sub_index;
        for(const auto& sub_pair : pair.second) {
            sub_index[sub_pair.first] = sub_pair.second;
        }

        std::string subtree_hash = buildTree(sub_index);
        tree_entries.push_back({"040000", subtree_hash, pair.first});
    }

    return writeTreeObject(tree_entries);
}


std::string buildTreeFromIndex() {
    fs::path indexPath = ".cgit/index";
    std::map<std::string, std::string> index_map;
    
    if (fs::exists(indexPath)) {
        std::ifstream index_in(indexPath);
        std::string line;
        while(std::getline(index_in, line)) {
            std::stringstream ss(line);
            std::string mode, hash, path;
            ss >> mode >> hash >> path;
            index_map[path] = hash;
        }
    }
    
    if (index_map.empty()) {
        throw std::runtime_error("Index is empty. Nothing to commit.");
    }
    
    return buildTree(index_map);
}

std::string writeTreeObject(const std::vector<IndexEntry>& entries) {
    std::vector<IndexEntry> sorted_entries = entries;
    std::sort(sorted_entries.begin(), sorted_entries.end(), [](const IndexEntry& a, const IndexEntry& b) {
        return a.path < b.path;
    });

    std::stringstream tree_content;
    for (const auto& entry : sorted_entries) {
        std::string type = (entry.mode == "040000") ? "tree" : "blob";
        tree_content << entry.mode << " " << type << " " << entry.hash << "\t" << fs::path(entry.path).filename().string() << "\n";
    }

    std::string content = tree_content.str();
    if (!content.empty()) {
        content.pop_back();
    }
    
    std::string object_str = "tree " + std::to_string(content.length()) + '\0' + content;
    
    std::string sha1_hash = calculate_sha1(object_str);
    std::string compressed_content = compress_string(object_str);
    
    std::string dir_name = sha1_hash.substr(0, 2);
    std::string file_name = sha1_hash.substr(2);
    fs::path object_dir = fs::path(".cgit") / "objects" / dir_name;
    fs::create_directories(object_dir);
    
    fs::path object_path = object_dir / file_name;
    std::ofstream object_file(object_path, std::ios::binary);
    object_file.write(compressed_content.c_str(), compressed_content.length());

    return sha1_hash;
}