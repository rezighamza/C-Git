#pragma once

#include <string>

void handleInitCommand();
void handleAddCommand(const std::string& filePath);
void handleCommitCommand(const std::string& message);
// We can also add hash-object here if we want to keep it
void handleHashObjectCommand(const std::string& filePath);