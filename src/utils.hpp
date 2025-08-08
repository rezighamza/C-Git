#pragma once 
#include <string>

// Calculates the SHA-1 hash of a string and returns it as a 40-char hex string.
std::string calculate_sha1(const std::string& content);

// Compresses a string using zlib and returns the compressed result.
std::string compress_string(const std::string& str);