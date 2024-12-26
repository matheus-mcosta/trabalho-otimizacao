#include "heuristic.h"
#include "utils.h"
#include <iostream>
#include <limits>
#include <string>

// Declare the genetic_algorithm function from heuristic.cpp
Solution genetic_algorithm(const Instance &instance, unsigned int seed,
                           unsigned int max_iterations, double time_limit);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_file> <seed> [-i max_iterations] [-t "
                     "time_limit]"
                  << std::endl;
        return 1;
    }

    try {
        // Required arguments
        std::string input_file = argv[1];
        unsigned int seed = static_cast<unsigned int>(std::stoul(argv[2]));

        // Optional arguments with default values
        unsigned int max_iterations = std::numeric_limits<unsigned int>::max();
        double time_limit = std::numeric_limits<double>::infinity();

        // Parse optional flags
        for (int i = 3; i < argc; i += 2) {
            std::string flag = argv[i];
            if (i + 1 >= argc) {
                std::cerr << "Missing value for " << flag << std::endl;
                return 1;
            }

            if (flag == "-i") {
                max_iterations =
                    static_cast<unsigned int>(std::stoul(argv[i + 1]));
            } else if (flag == "-t") {
                time_limit = std::stod(argv[i + 1]);
            } else {
                std::cerr << "Unknown flag: " << flag << std::endl;
                return 1;
            }
        }

        // Read the instance from the input file
        Instance instance = read_instance(input_file);

        // Call the genetic algorithm function
        Solution best_solution =
            genetic_algorithm(instance, seed, max_iterations, time_limit);

        // Output the best solution found
        std::cout << "Best solution fitness: " << best_solution.fitness
                  << std::endl;
        // Print as csv with: elapsed time,first_solution, best solution
        // fitness, and iteration for later analysis
        std::cout << best_solution.elapsed_time << ","
                  << best_solution.first_solution << ","
                  << best_solution.fitness << "," << best_solution.iteration
                  << std::endl;

        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
