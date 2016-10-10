//
// Created by marandil on 30.09.16.
//

#include <cstdlib>
#include <deque>
#include <future>

#include "gambler.hpp"
#include "bit_tracker.hpp"
#include "precomputed_prob_function.hpp"

#include "los_rng.hpp"

#include "mdlutils/multithreading/thread_pool.hpp"

#include <openssl/sha.h>

int concurrency = 4; // hint: std::thread::hardware_concurrency()

std::deque<std::future<bool>> all_tasks;
mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::dynamic);

std::string ADD_BASE = "GAMBLER_0001";

integer kdf(int N, int i, uint64_t idx, uint64_t runs)
{
    std::string input = ADD_BASE + "#" + std::to_string(idx + (i * runs) + 1); // +1 to match Julia indices
    unsigned char buffer[32];
    SHA256(reinterpret_cast<const unsigned char*>(input.data()), input.size(), buffer);
    std::string key = mdl::hexify(buffer, 32);
    //std::cout << "input : " << input << "\t";
    //std::cout << "KEY : " << key << "\n";
    return integer(key, 16);
}

bool step_six(prob_function p, std::string pd, prob_function q, std::string qd, int N, int i,
              std::function<sim_function(bit_function)> sim, std::string simd,
              std::function<bit_function(integer)> gen, std::string gend, uint64_t idx, uint64_t runs)
{
    //std::cout << "six start\n";
    bit_function bf = gen(kdf(N, i, idx, runs));
    sim_function sf = sim(bf);
    
    gambler::Gambler1D G(i, N, p, q);
    auto pair = G.run_gambler(sf);
    
    //std::cout << "six end\n";
    printf("%s;%s;%d;%d;%s;%s;%s;%zd;\n", simd.c_str(), gend.c_str(), i, N, pd.c_str(), qd.c_str(), pair.first ? "true" : "false", pair.second);
    
    return true;
}

void step_five(prob_function p, std::string pd, prob_function q, std::string qd, int N, int i,
               std::function<sim_function(bit_function)> sim, std::string simd,
               std::function<bit_function(integer)> gen, std::string gend)
{
    uint64_t runs = 1024*1024;
    
    for(uint64_t idx = 0; idx < runs; ++idx)
    {
        //all_tasks.emplace_back(std::async(step_six, p, pd, q, qd, N, i, sim, simd, gen, gend, idx, runs));
        //step_six(p, pd, q, qd, N, i, sim, simd, gen, gend, idx, runs);
        if(pool.get_awaiting_tasks() > 65536)
            while(pool.get_awaiting_tasks() > 1024)
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
        all_tasks.emplace_back(pool.async(step_six, p, pd, q, qd, N, i, sim, simd, gen, gend, idx, runs));
    }
}

void step_four(prob_function p, std::string pd, prob_function q, std::string qd, int N, int i,
               std::function<sim_function(bit_function)> sim, std::string simd)
{
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("RANDU"), "RandU LCG");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("Mersenne"), "Mersenne Twister");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("MersenneAR"), "Mersenne AR");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("VS"), "Visual Studio");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("C_PRG"), "C Rand");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("Rand"), "Old BSD");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("Minstd"), "Minstd");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("Borland"), "Borland C");
    step_five(p, pd, q, qd, N, i, sim, simd, los_rng::get_gen("CMRG"), "CMRG");
}

void step_three(prob_function p, std::string pd, prob_function q, std::string qd, int N, int i)
{
    auto BT = [](bit_function func)
        { return bit_tracker::BitTracker(func); };
    
    step_four(p, pd, q, qd, N, i, BT, "BitTracker");
}

// second entry point, tweak starting points (i)
void step_two(prob_function p, std::string pd, prob_function q, std::string qd, int N)
{
    int i = N / 2;
    
    step_three(p, pd, q, qd, N, i);
}

// first entry point, tweak tested p/q pairs here and max. number of states (N)
void step_one()
{
    int N = 128;
    
    // delta - deviation from 0.5 for the upcoming tests:
    std::string delta = "(1//64)";
    
    {
        // new test: (atan(N/2 - i) / 32pi) + 0.5 (values range between 31/64 and 33/64)
        auto p = julia_prob_function("(atan(N / 2 - i) / pi) * " + delta + " + 0.5", N);
        auto q = p.get_negative();
        
        std::string pd = "atan(..)";
        std::string qd = "atan(..)";
        step_two(p, pd, q, qd, N);
    }
    
    {
        // new test: (sin((i / N) * 2pi) / delta + 0.5)
        auto p = julia_prob_function("0.5 + (sin((i / N) * 2pi) * " + delta + ")", N);
        auto q = p.get_negative();
        
        std::string pd = "sin(..)";
        std::string qd = "sin(..)";
        step_two(p, pd, q, qd, N);
    }
}

int main(int argc, const char *argv[])
{
    printf("sim;bs;i;N;p;q;won;len;\n");
    step_one();
    
    for(auto& f : all_tasks) f.wait();
}
