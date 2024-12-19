#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

struct BinBounds {
    int min_capacity;
    int max_capacity;
};

struct Instance {
    int num_bins;
    int num_balls;
    std::vector<BinBounds> bin_bounds;
};

Instance read_instance(const std::string& filename) {
    Instance instance;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Read number of bins and balls
    file >> instance.num_bins;
    file >> instance.num_balls;

    // Read bounds for each bin
    instance.bin_bounds.resize(instance.num_bins);
    for (int i = 0; i < instance.num_bins; i++) {
        file >> instance.bin_bounds[i].min_capacity;
        file >> instance.bin_bounds[i].max_capacity;
        
        if (instance.bin_bounds[i].min_capacity > instance.bin_bounds[i].max_capacity) {
            throw std::runtime_error("Invalid bounds for bin " + std::to_string(i));
        }
    }

    file.close();
    return instance;
}

// Helper function to print instance details
void print_instance(const Instance& instance) {
    std::cout << "Number of bins: " << instance.num_bins << "\n";
    std::cout << "Number of balls: " << instance.num_balls << "\n";
    std::cout << "Bin bounds:\n";
    for (int i = 0; i < instance.num_bins; i++) {
        std::cout << "Bin " << i << ": [" 
                 << instance.bin_bounds[i].min_capacity << ", " 
                 << instance.bin_bounds[i].max_capacity << "]\n";
    }
}