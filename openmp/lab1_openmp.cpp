#include <omp.h>
#include <iostream> 
#include <cstdint>
#include <chrono>
#include "sha256.h"
#include <thread>
#include <mutex>

bool flag = false;
std::string solve_crypto_puzzle(std::string str, uint puzzle_difficulty, int current_thread_id,int total_threads)
{
    std::string nonce_needle(puzzle_difficulty, '0');

    SHA256 sha256;
    for (uint64_t i = current_thread_id; i < UINT64_MAX; i+= total_threads) 
    {
        if (!flag)
        {
            std::string solution_candidate = str + std::to_string(i);
            std::string hash_code = sha256(solution_candidate);
            if (hash_code.compare(0, puzzle_difficulty, nonce_needle) == 0)
            {
                return solution_candidate;
            }
        }
        else throw "result found";
    }
    throw "No result found";
}


int main (int argc, char *argv[]) 
{


    int difficulty = atoi(argv[1]);
    SHA256 sha256;
    const std::string message("Hello World");

    std::cout << "Message:" << std::endl << message << std::endl;
    std::cout << "Hash:" << std::endl << sha256(message) << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "Looking for nonce to solve crypto-puzzle with " << difficulty << " difficulty" << "..." << std::endl;
    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "Application with OpenMP parameters:" << std::endl;

    int total_threads, current_thread_id;
    #pragma omp parallel num_threads(32) private(total_threads, current_thread_id) shared(flag)
    {
        current_thread_id = omp_get_thread_num();
        total_threads = omp_get_num_threads();
        std::cout << "Total threads: " << total_threads << std::endl;
        std::cout << "Current thread: " << current_thread_id << std::endl;
        try
        {
            auto solution = solve_crypto_puzzle(message, difficulty, current_thread_id, total_threads);
#pragma omp critical
            {
                if (!flag)
                {
                    std::cout << "Solution: " << std::endl << solution << std::endl;
                    std::cout << "Hash:" << std::endl << sha256(solution) << std::endl;
                    std::cout << "Current thread: " << current_thread_id << std::endl;
                    auto t2 = std::chrono::high_resolution_clock::now();
                    auto duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
                    std::cout << duration_milliseconds.count() << " milliseconds\n";
                    flag = true;
                }
            }
        }
        catch (const char* msg)
        {
            std::cout << msg << std::endl;
        }
    }
}
