//
// Created by marandil on 30.09.16.
//

#include <cstdlib>
#include <deque>
#include <future>
#include <mdlutils/types/range.hpp>

#include "gambler.hpp"

#include "mdlutils/multithreading/thread_pool.hpp"

#include "functions.hpp"
#include "generators.hpp"

int concurrency = 4; // hint: std::thread::hardware_concurrency()

// default value for KDF_PREFIX
std::string KDF_PREFIX = "GAMBLER_0001";

std::deque<std::future<bool>> all_tasks;
mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::dynamic);
//mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::power2choices);


template <typename StartsGen>
void runner(int N, int runs, StartsGen is, functions fun, std::string generator)
{
    auto single_runner = [=](int idx, int i)
        {
            auto gen = select_generator(generator, N, i, idx, runs);
            auto ppf = select_function(N, fun);
        
            gambler::Gambler1D G(i, N, ppf.p, ppf.q);
            auto pair = G.run_gambler(*gen.second);

            printf("%s;%s;%d;%d;%s;%s;%s;%zd;\n", "BitTracker", gen.first.c_str(), i, N, ppf.pd.c_str(), ppf.qd.c_str(), pair.first ? "true" : "false", pair.second);
        
            return true;
        };
    
    for(int i : is)
        for(int idx : mdl::range<int>(runs))
        {
            if (pool.get_awaiting_tasks() > 65536)
                while (pool.get_awaiting_tasks() > 1024)
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
            all_tasks.emplace_back(pool.async(single_runner, idx, i));
        }
}

void setup_and_run()
{
    int N = 128;
    int runs = 16;
    
    // select a single function, to initialize the function cache.
    select_function(N, functions::SIN);
    
    std::vector<std::string> gens = {
            "RC4",
            "AES128CBC", "AES192CBC", "AES256CBC",
            "AES128CTR", "AES192CTR", "AES256CTR",
            "RANDU", "Mersenne", "MersenneAR", "VS", "C_PRG", "Rand", "Minstd", "Borland", "CMRG"
    };
    
    std::vector<functions> funs = {
            functions::T1,
            functions::T2,
            functions::T3,
            functions::T4
    };
    
    std::vector<int> is = {
            N/2
    };
    
    for(auto& gen : gens)
        for(auto& fun : funs)
            runner(N, runs, is, fun, gen);
}

int main(int argc, const char *argv[])
{
    // read in the gambler-prefix for KDF:
    const char* _env = std::getenv("KDF_PREFIX");
    if(_env)
        KDF_PREFIX = _env;
    
    printf("sim;bs;i;N;p;q;won;len;\n");
    setup_and_run();
    
    for(auto& f : all_tasks) f.wait();
}
