//
// Created by marandil on 30.09.16.
//

#include <cstdlib>
#include <deque>
#include <future>
#include <fstream>

#include <mdlutils/types/range.hpp>
#include <mdlutils/multithreading/thread_pool.hpp>

#include "gambler.hpp"

#include "functions.hpp"
#include "generators.hpp"
#include "statistics.hpp"

unsigned concurrency = 4; // hint: std::thread::hardware_concurrency()

// default value for KDF_PREFIX
std::string KDF_PREFIX = "GAMBLER_0001";
// DUMP_FUNCTIONS environmental variable, if not empty then a dump file for the function should be created
std::string DUMP_FUNCTIONS = "";

std::deque<std::future<bool>> all_tasks;
mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::dynamic);
//mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::power2choices);


template <typename StartsGen>
void runner(unsigned N, size_t runs, StartsGen is, functions fun, std::string generator)
{
    auto single_runner = [N, runs, fun, generator](size_t idx, unsigned i)
        {
            auto gen = select_generator(generator, N, i, idx, runs);
            auto ppf = select_function(N, fun);
        
            gambler::Gambler1D G(i, N, ppf.p, ppf.q);
            auto pair = G.run_gambler(*gen.second);

            printf("%s;%s;%d;%d;%s;%s;%s;%zd;\n", "BitTracker", gen.first.c_str(), i, N, ppf.pd.c_str(), ppf.qd.c_str(), pair.first ? "true" : "false", pair.second);
            
            return true;
        };
    
    for(unsigned i : is)
        for(size_t idx : mdl::range<size_t>(runs))
        {
            if (pool.get_awaiting_tasks() > 65536)
                while (pool.get_awaiting_tasks() > 1024)
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
            all_tasks.emplace_back(pool.async(single_runner, idx, i));
        }
}

void dump_functions(int N, functions fn)
{
    std::ofstream of(DUMP_FUNCTIONS);
    of << "p;q;N;i;EX;VarX;ET;VarT\n";
    
    auto ppf = select_function(N, fn);
    std::vector<statistics> stat = compute_expected_for_all(*ppf.p, ppf.pd, *ppf.q, ppf.qd, N, true);
    // for each starting point i
    for(int i=1; i < N; ++i)
    {
        // compute the variance V
        rational P = trim_precision(stat[i].win_prob, 64);
        rational V = trim_precision(P*(1-P), 64);
        
        of << ppf.pd << ";" << ppf.qd << ";" << N << ";" << i << ";";
        of << "=" << P.get_num() << "/" << P.get_den() << ";";
        of << "=" << V.get_num() << "/" << V.get_den() << ";";
        
        rational T = trim_precision(stat[i].time_expected, 64);
        rational W = trim_precision(stat[i].time_variance, 64);
        of << "=" << T.get_num() << "/" << T.get_den() << ";";
        of << "=" << W.get_num() << "/" << W.get_den() << ";\n";
    }
}

void setup_and_run_regular()
{
    unsigned N = 128;
    unsigned runs = 16;
    
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
    
    std::vector<unsigned> is = {
            N/2
    };
    
    for(auto& gen : gens)
        for(auto& fun : funs)
            runner(N, runs, is, fun, gen);
}

void setup_and_run_tests()
{
    // no. of states
    int N = 129;
    // select a single function, to initialize the function cache.
    select_function(N, functions::SIN);
    
    //rational b_inv = 1000000_mpq / 5_mpq;
    rational z = 196_mpq / 100_mpq;
    rational z_sq = z * z;
    
    std::vector<std::string> gens = {
    //        "RC4",
    //        "AES128CBC", "AES192CBC", "AES256CBC",
    //        "AES128CTR", "AES192CTR", "AES256CTR",
    //        "RANDU", "Mersenne", "MersenneAR", "VS", "C_PRG", "Rand", "Minstd", "Borland", "CMRG"
            "AES256CTR", "RANDU"
    };
    
    std::vector<functions> funs = {
            functions::T1,
            functions::T2,
            functions::T3,
            functions::T4,
    };
    
    for(functions fn : funs)
    {
        if(DUMP_FUNCTIONS != "")
            dump_functions(N, fn);
        else
        {
            auto ppf = select_function(N, fn);
            std::vector<statistics> stat = compute_expected_for_all(*ppf.p, ppf.pd, *ppf.q, ppf.qd, N, false);
    
            // for each starting point i
            for (int i = 1; i < N; ++i)
            {
                // compute the variance V
                rational P = trim_precision(stat[i].win_prob, 64);
                rational V = trim_precision(P * (1 - P), 64);
    
                // extract the time variance W
                rational T = trim_precision(stat[i].time_expected, 64);
                rational W = trim_precision(stat[i].time_variance, 64);
    
                // get b1 and b2 of roughly 1/10000 of their corresponding expected values
                rational b1 = P / 100_mpq; b1 = b1 ? b1 : 1_mpq/100000_mpq; // set b1, b2 to anything if P or T
                rational b2 = T / 100_mpq; b2 = b2 ? b2 : 1_mpq/100000_mpq; // is equal to 0
                rational b1_inv = 1_mpq/b1;
                rational b2_inv = 1_mpq/b2;
                rational z_sq_b1_inv_sq = b1_inv * b1_inv * z_sq;
                rational z_sq_b2_inv_sq = b2_inv * b2_inv * z_sq;
    
                // compute n_1 s.t. b_1 is satisfied:
                rational n1 = V * z_sq_b1_inv_sq;
                // round down and add 1 to simplify computations
                size_t runs1 = size_t(n1.get_d()) + 1;
                
                // compute n_2 s.t. b_2 is satisfied:
                rational n2 = W * z_sq_b2_inv_sq;
                // round down and add 1 for a simpler math
                size_t runs2 = size_t(n2.get_d()) + 1;
                
                // if any of the runs1/runs2 would be greater than 2^20, print warning and clip runs
                if(runs1 > 1<<20)
                {
                    std::cerr << "Too many runs required for i: " << i << ",\tb_1: " << b1.get_d() << ", clipping...\n";
                    runs1 = 1<<20;
                    b1 = rsqrt(V / runs1) * z;
                }
                if(runs2 > 1<<20)
                {
                    std::cerr << "Too many runs required for i: " << i << ",\tb_2: " << b2.get_d() << ", clipping...\n";
                    runs2 = 1<<20;
                    b2 = rsqrt(V / runs2) * z;
                }
                // print runs1 and runs2:
                std::cerr << "for i: " << i << ",\tb_1: " << b1.get_d() << ",\tb_2: " << b2.get_d();
                std::cerr << ",\tn_1: " << runs1 << ",\tn_2: " << runs2 << "\n";
        
                for (auto &gen : gens)
                {
                    runner(N, runs1 > runs2 ? runs1 : runs2, std::vector<int>{i}, fn, gen);
                }
            }
        }
    }
}

int main(int argc, const char *argv[])
{
    const char* _env;
    // read in the gambler-prefix for KDF:
    _env = std::getenv("KDF_PREFIX");
    if(_env)
        KDF_PREFIX = _env;
    // read in the dump functions filename:
    _env = std::getenv("DUMP_FUNCTIONS");
    if(_env)
        DUMP_FUNCTIONS = _env;
    
    printf("sim;bs;i;N;p;q;won;len;\n");
    //setup_and_run_regular();
    setup_and_run_tests();
    
    for(auto& f : all_tasks)
        f.wait();
}
