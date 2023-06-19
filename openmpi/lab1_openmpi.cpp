#include <mpi.h>
#include "sha256.h"
#include <iostream> 
#include <cstdint>
#include <chrono>

bool flag = false;
std::string solve_crypto_puzzle(std::string str, uint puzzle_difficulty, int current_thread_id, int total_threads)
{
    std::string nonce_needle(puzzle_difficulty, '0');
    std::string empty;
    SHA256 sha256;
    MPI_Status status;

    for (uint64_t i = current_thread_id; i < UINT64_MAX; i += total_threads)
    {
        if (!flag)
        {
            std::string solution_candidate = str + std::to_string(i);
            std::string hash_code = sha256(solution_candidate);
            if (hash_code.compare(0, puzzle_difficulty, nonce_needle) == 0)
            {
                flag = true;	
                return solution_candidate;
            }
        }
    }
    return empty;
}

int main(int argc, char* argv[])
{

    int difficulty = 7;
    SHA256 sha256;
    const std::string message("Hello World");

    std::cout << "Message:" << std::endl << message << std::endl;
    // std::cout << "Hash:" << std::endl << sha256(message) << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "Looking for nonce to solve crypto-puzzle with " << difficulty << " difficulty" << "..." << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();

    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size=5;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    std::cout << "Hello world from processor " << processor_name << ", rank " << world_rank << " out of " << world_size << " processors" << std::endl;
        auto solution = solve_crypto_puzzle(message, difficulty, world_rank, world_size);
            if (!solution.empty())
            {
                std::cout << "Solution: " << std::endl << solution << std::endl;
                std::cout << "Hash:" << std::endl << sha256(solution) << std::endl;
                std::cout << "Current processor: " << world_rank << std::endl;
                auto t2 = std::chrono::high_resolution_clock::now();
                auto duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                std::cout << duration_milliseconds.count() << " milliseconds\n";
                flag = true;
				MPI_Abort(MPI_COMM_WORLD, 0);
            }
    MPI_Finalize();
}
