#!/bin/bash
set -e

make clean
make

TIME_LIMITS=(5 300)
INSTANCES=$(printf "%02d " {1..10})
SEEDS=$(seq 1 10)

mkdir -p results

for i in $INSTANCES; do
    for t in "${TIME_LIMITS[@]}"; do
        output_file="results/instance${i}_t${t}.csv"
        echo "seed,time,first_solution,best_solution,iteration" > "$output_file"
        
        for s in $SEEDS; do
            instance_file="../instances/${i}.txt"
            
            echo "Running instance $i with seed $s and time limit $t"
            output=$(./genetic_solver "$instance_file" "$s" -t "$t" | tail -n 1)
            echo "$s,$output" >> "$output_file"
        done
    done
done

echo "All runs completed!"