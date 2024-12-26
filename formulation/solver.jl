using JuMP
using HiGHS
using Statistics

function read_input(filename)
    open(filename) do f
        bins = parse(Int, readline(f))    # número de recipientes
        balls = parse(Int, readline(f))    # número de bolas

        bounds = Vector{Tuple{Int,Int}}(undef, bins)
        for i in 1:bins
            l, u = parse.(Int, split(readline(f)))
            bounds[i] = (l, u)
        end

        return bins, balls, bounds
    end
end


function solve_balls_and_bins(filename::String, seed::Int, time_limit::Float64)

    n, m, bounds = read_input(filename)
    l = [bounds[i][1] for i in 1:n]
    u = [bounds[i][2] for i in 1:n]

    model = Model(HiGHS.Optimizer)

    set_attribute(model, "random_seed", seed)
    set_attribute(model, "time_limit", time_limit)

    @variable(model, 0 <= x[i=1:n] <= u[i], Int)  # bolas por bin
    @variable(model, z[i=1:n, j=1:maximum(u)], Bin)  # binario que indica se o recipiente i tem no minimo j bolas

    # objetivo: max o lucro
    @objective(model, Max, sum(j * z[i, j] for i in 1:n, j in 1:u[i]))

    # 1. relacionar x[i] com z[i, j]
    @constraint(model, [i=1:n], x[i] == sum(z[i, j] for j in 1:u[i]))

    # 2. garantir limites inferior e superior de cada recipiente
    @constraint(model, [i=1:n], l[i] <= x[i] <= u[i])

    # 3. garantir consistencia de z[i, j] (z[i, j] = 1 if x[i] >= j)
    @constraint(model, [i=1:n, j=2:u[i]], z[i, j] <= z[i, j - 1])

    # 4. numero total de bolas deve ser igual a m
    @constraint(model, sum(x[i] for i in 1:n) == m)

    # Resolve o modelo
    start_time = time()
    optimize!(model)
    solve_time = time() - start_time
    println(sum(value.(x)))


    # Coleta resultados
    status = termination_status(model)
    objective = if has_values(model)
        Int(objective_value(model))
    else
        NaN
    end

    solution = if has_values(model)
        value.(x)
    else
        Float64[]
    end

    # Verifica a solução
    if has_values(model)
        println("\nVerificação da solução:")
        println("Objetivo do modelo: $objective")

        # Verifica restrições
        total_balls = sum(solution)
        println("\nVerificação das restrições:")
        println("Total de bolas utilizadas: $total_balls (maximo de $m)")
        println("Todos os limites respeitados: ", all(bounds[i][1] <= solution[i] <= bounds[i][2] for i in 1:n))
    end
    return Dict(
        "status" => status,
        "objective" => objective,
        "time" => solve_time,
        "solution" => solution
    )
end

function run_multiple_seeds(filename::String, time_limit::Float64, seeds::Vector{Int})
    results = []

    for seed in seeds
        println("\nExecutando com semente $seed:")
        result = solve_balls_and_bins(filename, seed, time_limit)
        push!(results, result)

        println("Status: $(result["status"])")
        println("Objetivo: $(result["objective"])")
        println("Tempo: $(result["time"]) segundos")
        println("-"^50)
    end

    # Calcula e exibe médias
    valid_results = filter(r -> !isnan(r["objective"]), results)
    if !isempty(valid_results)
        avg_objective = mean(r["objective"] for r in valid_results)
        avg_time = mean(r["time"] for r in valid_results)

        println("\nEstatísticas para $(length(valid_results)) execuções válidas:")
        println("Objetivo médio: $avg_objective")
        println("Tempo médio: $avg_time segundos")
    else
        println("\nNenhuma execução válida encontrada!")
    end

    return results
end

# Execução principal
if length(ARGS) < 3
    println("Uso: julia solver.jl <arquivo_entrada> <semente> <limite_tempo>")
    println("Ou:  julia solver.jl <arquivo_entrada> all <limite_tempo> (para executar sementes 1-10)")
    exit(1)
end

input_file = ARGS[1]
seed_arg = ARGS[2]
time_limit = parse(Float64, ARGS[3])

try
    if seed_arg == "all"
        results = run_multiple_seeds(input_file, time_limit, collect(1:10))
    else
        seed = parse(Int, seed_arg)
        result = solve_balls_and_bins(input_file, seed, time_limit)
        println("Status: $(result["status"])")
        println("Objetivo: $(result["objective"])")
        println("Tempo: $(result["time"]) segundos")
    end
catch e
    println("Erro durante a execução:")
    println(e)
    exit(1)
end
