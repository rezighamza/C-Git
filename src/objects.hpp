#pragma once

#include <string>
#include <vector>
#include <map>

// Helper struct to represent an entry in a tree or the index
struct IndexEntry {
    std::string mode;
    std::string hash;
    std::string path;
};

// Takes file content, creates a blob, stores it, and returns the hash.
std::string writeBlob(const std::string& content);

// Takes a list of tree entries, creates a tree object, and returns its hash.
std::string writeTreeObject(const std::vector<IndexEntry>& entries);

// Reads the index, builds the complete tree hierarchy, and returns the root tree hash.
std::string buildTreeFromIndex();