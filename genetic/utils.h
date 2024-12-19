#pragma once

#include <string>
#include <vector>

struct BinBounds {
    int min_capacity;
    int max_capacity;
};

struct Instance {
    int num_bins;
    int num_balls;
    std::vector<BinBounds> bin_bounds;
};

Instance read_instance(const std::string& filename);
void print_instance(const Instance& instance);