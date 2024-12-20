#include <iostream>
#include <string>
#include <limits>
#include "utils.h"
#include "heuristic.h"

// Declare the genetic_algorithm function from heuristic.cpp
Solution genetic_algorithm(const Instance& instance, unsigned int seed, unsigned int max_iterations, double time_limit);

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_file> <seed> <max_iterations> [<time_limit>]" << std::endl;
        return 1;
    }

    try {
        // Parse command-line arguments
        std::string input_file = argv[1];
        unsigned int seed = static_cast<unsigned int>(std::stoul(argv[2]));
        unsigned int max_iterations = static_cast<unsigned int>(std::stoul(argv[3]));
        double time_limit = std::numeric_limits<double>::infinity();

        if (argc >= 5) {
            time_limit = std::stod(argv[4]);
        }

        // Read the instance from the input file
        Instance instance = read_instance(input_file);

        // Call the genetic algorithm function
        Solution best_solution = genetic_algorithm(instance, seed, max_iterations, time_limit);

        // Output the best solution found
        std::cout << "Best solution fitness: " << best_solution.fitness << std::endl;
        // std::cout << "Ball assignments:" << std::endl;
        // for (size_t i = 0; i < best_solution.ball_assignments.size(); ++i) {
        //     std::cout << "Ball " << i << " assigned to Bin " << best_solution.ball_assignments[i] << std::endl;
        // }

        // Output any additional information needed for the report
        // ...

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}