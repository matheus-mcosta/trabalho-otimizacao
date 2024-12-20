#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "utils.h"
#include <vector>

// Define the Solution structure
struct Solution {
    std::vector<int> ball_assignments; // ball_assignments[ball] = bin
    int fitness; // Objective function value
};

// Declaration of the genetic_algorithm function
Solution genetic_algorithm(const Instance& instance, unsigned int seed, unsigned int max_iterations, double time_limit);
#endif // HEURISTIC_H