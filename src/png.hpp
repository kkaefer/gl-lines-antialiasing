#pragma once

#include <string>

void write_file(const std::string &filename, const std::string &data);
std::string read_file(const std::string &filename);

std::string compress_png(int width, int height, void *rgba);
