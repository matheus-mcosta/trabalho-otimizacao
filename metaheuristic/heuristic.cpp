#include "utils.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

// Define the Solution structure
struct Solution {
    std::vector<int> ball_assignments; // ball_assignments[ball] = bin
    int fitness;                       // Objective function value
    int first_solution;                // First solution value
    int iteration;                     // Iteration
    double elapsed_time;               // Elapsed time
};

// Evaluate the fitness of a solution
int evaluate_fitness(const Solution &sol, const Instance &instance) {
    int fitness = 0;
    std::vector<int> bin_counts(instance.num_bins, 0);

    // Count balls in each bin
    for (int ball = 0; ball < instance.num_balls; ++ball) {
        ++bin_counts[sol.ball_assignments[ball]];
    }

    // Calculate fitness based on bin loads
    for (int bin = 0; bin < instance.num_bins; ++bin) {
        int load = bin_counts[bin];
        const auto &bounds = instance.bin_bounds[bin];
        int max_capacity = bounds.max_capacity;
        int min_capacity = bounds.min_capacity;

        if (load > max_capacity) {
            // Penalize if load exceeds max capacity
            int penalty = (load - max_capacity);
            fitness -= penalty * (penalty + 1) / 2;
        } else if (load < min_capacity) {
            // Penalize if load is less than min capacity
            int penalty = (min_capacity - load);
            fitness -= penalty * (penalty + 1) / 2;
        } else {
            // Reward if load is within the valid range
            fitness += load * (load + 1) / 2;
        }
    }

    return fitness;
}

// Initialize the population

std::vector<Solution> initialize_population(int population_size,
                                            const Instance &instance,
                                            std::mt19937 &rng) {
    std::vector<Solution> population;
    population.reserve(
        population_size); // Preallocate memory for the population

    std::uniform_int_distribution<int> bin_dist(0, instance.num_bins - 1);

    auto create_solution = [&](bool random) -> Solution {
        Solution sol;
        sol.ball_assignments.resize(instance.num_balls);
        for (int b = 0; b < instance.num_balls; ++b) {
            sol.ball_assignments[b] =
                random ? bin_dist(rng) : (b % instance.num_bins);
        }
        sol.fitness = evaluate_fitness(sol, instance);
        sol.first_solution =
            (population.empty()) ? sol.fitness : population[0].first_solution;
        return sol;
    };

    // Random initialization (first half of population)
    for (int i = 0; i < population_size / 2; ++i) {
        population.push_back(create_solution(true));
    }

    // Heuristic-based initialization (second half of population)
    for (int i = population_size / 2; i < population_size; ++i) {
        population.push_back(create_solution(false));
    }

    return population;
}

// Enhanced Selection operator (roulette wheel selection)
Solution selection(const std::vector<Solution> &population, std::mt19937 &rng) {
    std::vector<double> cumulative_fitness(population.size());
    cumulative_fitness[0] = population[0].fitness;
    for (size_t i = 1; i < population.size(); ++i) {
        cumulative_fitness[i] =
            cumulative_fitness[i - 1] + population[i].fitness;
    }

    std::uniform_real_distribution<double> dist(0, cumulative_fitness.back());
    double random_value = dist(rng);

    auto it = std::lower_bound(cumulative_fitness.begin(),
                               cumulative_fitness.end(), random_value);
    return population[std::distance(cumulative_fitness.begin(), it)];
}

// Crossover operator (uniform crossover)
Solution crossover(const Solution &parent1, const Solution &parent2,
                   std::mt19937 &rng) {
    Solution offspring;
    offspring.ball_assignments.resize(parent1.ball_assignments.size());

    // Select two random crossover points
    std::uniform_int_distribution<size_t> dist(
        0, parent1.ball_assignments.size() - 1);
    size_t point1 = dist(rng);
    size_t point2 = dist(rng);

    // Ensure point1 < point2
    if (point1 > point2)
        std::swap(point1, point2);

    // Copy genes from parent1 and parent2
    for (size_t i = 0; i < parent1.ball_assignments.size(); ++i) {
        if (i < point1 || i >= point2) {
            offspring.ball_assignments[i] = parent1.ball_assignments[i];
        } else {
            offspring.ball_assignments[i] = parent2.ball_assignments[i];
        }
    }

    return offspring;
}

// Adaptive Mutation operator
void mutation(Solution &sol, const Instance &instance, std::mt19937 &rng,
              double mutation_rate) {
    std::uniform_int_distribution<int> ball_dist(0, instance.num_balls - 1);
    std::uniform_int_distribution<int> bin_dist(0, instance.num_bins - 1);

    if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) < mutation_rate) {
        int ball = ball_dist(rng);
        int new_bin = bin_dist(rng);
        sol.ball_assignments[ball] = new_bin;
    }
}

// Genetic algorithm main function
Solution genetic_algorithm(const Instance &instance, unsigned int seed,
                           unsigned int max_iterations, double time_limit) {
    std::mt19937 rng(seed);
    auto start_time = std::chrono::steady_clock::now();

    const int population_size = 10;
    const double crossover_rate = 0.2;
    const int elite_size = 3;
    double mutation_rate = 0.1;

    // Initialize population
    auto population = initialize_population(population_size, instance, rng);
    Solution best_solution = population[0];
    best_solution.first_solution = population[0].first_solution;

    // Evaluate fitness for all individuals
    for (auto &sol : population) {
        sol.fitness = evaluate_fitness(sol, instance);
    }

    // Sort population by fitness (descending)
    std::sort(population.begin(), population.end(),
              [](const Solution &a, const Solution &b) {
                  return a.fitness > b.fitness;
              });

    unsigned int iteration = 0;

    // Main loop of the genetic algorithm
    while (iteration < max_iterations) {

        std::vector<Solution> new_population;
        new_population.reserve(
            population_size); // Reserve space for new population
        // Elitism: Keep the top 'elite_size' individuals
        new_population.insert(new_population.end(), population.begin(),
                              population.begin() + elite_size);

        // Generate new individuals via selection, crossover, and mutation
        while (new_population.size() < population_size) {
            Solution parent1 = selection(population, rng);
            Solution parent2 = selection(population, rng);

            Solution offspring = parent1;
            if (std::uniform_real_distribution<double>(0.0, 1.0)(rng) <
                crossover_rate) {
                offspring = crossover(parent1, parent2, rng);
            }

            mutation(offspring, instance, rng, mutation_rate);
            offspring.fitness = evaluate_fitness(offspring, instance);
            new_population.push_back(offspring);
        }

        // Local search: try improving the best solution found so far
        Solution local_best = new_population[0];
        for (int i = 0; i < 100; ++i) {
            Solution neighbor = local_best;
            mutation(neighbor, instance, rng, mutation_rate);
            neighbor.fitness = evaluate_fitness(neighbor, instance);
            if (neighbor.fitness > local_best.fitness) {
                local_best = neighbor;
            }
        }

        new_population[0] = local_best;

        // Replace the old population with the new one
        population = std::move(new_population);

        // Track and print the best solution so far
        if (population[0].fitness > best_solution.fitness) {
            best_solution = population[0];
            auto current_time = std::chrono::steady_clock::now();
            double elapsed_time =
                std::chrono::duration<double>(current_time - start_time)
                    .count();
            std::cout << std::fixed << std::setprecision(2) << elapsed_time
                      << " " << best_solution.fitness << " Iteration "
                      << iteration << std::endl;
            best_solution.iteration = iteration;
            best_solution.elapsed_time =
                std::chrono::duration<double>(std::chrono::steady_clock::now() -
                                              start_time)
                    .count();
        }

        ++iteration;

        // Check for time limit
        auto current_time = std::chrono::steady_clock::now();
        if (std::chrono::duration<double>(current_time - start_time).count() >=
            time_limit) {
            break;
        }
    }

    return best_solution;
}
