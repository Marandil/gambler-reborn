//
// Created by marandil on 30.09.16.
//

#include <cassert>
#include <cstdlib>
#include <deque>
#include <future>
#include <fstream>
#include <map>

#include <mdlutils/types/range.hpp>
#include <mdlutils/multithreading/thread_pool.hpp>

#include "gambler.hpp"

#include "statistics.hpp"

unsigned concurrency = 4; // hint: std::thread::hardware_concurrency()

// default value for KDF_PREFIX
std::string KDF_PREFIX = "GAMBLER_0001";
// DUMP_FUNCTIONS environmental variable, if not empty then a dump file for the function should be created
std::string DUMP_FUNCTIONS = "";

std::list<std::future<bool>> all_tasks;
mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::dynamic);
//mdl::thread_pool pool(concurrency, mdl::thread_pool::strategy::power2choices);

auto kdf = kdf_unrelated;

bool binout = true;
constexpr size_t header_length = 128;

void rbin(size_t header, bool won, size_t length)
{
    struct packet {
        size_t hdr;
        size_t wl;
    } _value;
    size_t mask = won ? (1ull << (sizeof(size_t) * 8 - 1)) : 0;
    _value.wl = (length | mask);
    _value.hdr = header;
    fwrite(&_value, sizeof(packet), 1, stdout);
}

std::map<std::string, size_t> header_map;
std::mutex header_map_lock;

size_t reg_header(const std::string& header)
{
    std::unique_lock<std::mutex> lock(header_map_lock);
    auto it = header_map.find(header);
    if(it == header_map.end())
    {
        size_t idx = header_map.size();
        header_map[header] = idx;
    
        constexpr size_t max_hdr_length = header_length - sizeof(size_t);
    
        assert(header.size() < max_hdr_length);
        struct packet {
            size_t idx;
            char hdr[max_hdr_length] = {0};
        } _value;
    
        _value.idx = idx;
        strcpy(_value.hdr, header.c_str());
        fwrite(&_value, sizeof(packet), 1, stdout);
        
        return idx;
    }
    else
        return it->second;
}

bool single_runner(unsigned N, size_t runs, functions fun, std::string generator, size_t start, size_t end, unsigned i)
{
    auto ppf = select_function(N, fun);
    auto gen = select_generator(generator);
    bit_function_p bf = gen.second;
    bt_p bt = std::make_shared<bit_tracker::BitTracker>(bf);
    
    std::string header;
    {
        std::stringstream ss("BitTracker;");
        ss << gen.first << ";" << i << ";" << N << ";" << ppf.pd << ";" << ppf.qd << ";";
        //if(binout)
        //    ss << "{w};{l};";
        header = ss.str();
    }
    // not neccessary if not in binout mode
    size_t header_idx = binout ? reg_header(header) : 0;
    
    for(size_t idx : mdl::range<size_t>(start, end))
    {
        integer key = kdf(N, i, idx, runs);
        bf->set_seed(key);
        
        gambler::Gambler1D G(i, N, ppf.p, ppf.q);
        auto pair = G.run_gambler(bt);
    
        if(binout)
            rbin(header_idx, pair.first, pair.second);
        else
            printf("%s%s;%zd;\n", header.c_str(), pair.first ? "true" : "false", pair.second);
    
        gen.second = nullptr;
    }
    return true;
}

template <typename StartsGen>
void runner(unsigned N, size_t runs, StartsGen is, functions fun, std::string generator)
{
    constexpr size_t step_size = 1024;
    for(unsigned i : is)
        for(size_t start : mdl::range<size_t>(0, runs, step_size))
        {
            size_t end = std::min(start + step_size, runs);
            
            if (pool.get_awaiting_tasks() > 65536)
                while (pool.get_awaiting_tasks() > 1024)
                {
                    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    for(auto it = all_tasks.begin(); it != all_tasks.end(); ++it)
                    {
                        if (it->valid())
                            it = all_tasks.erase(it);
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            all_tasks.emplace_back(pool.async(single_runner, N, runs, fun, generator, start, end, i));
        }
}

void dump_functions(int N, functions fn)
{
    std::ofstream of(DUMP_FUNCTIONS);
    of << "p;q;N;i;EX;VarX;ET;VarT\n";
    
    auto ppf = select_function(N, fn);
    std::vector<statistics> stat = compute_expected_for_all(ppf, N, true);
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
//            functions::T2,
//            functions::T3,
//            functions::T4
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
            "RC4",
    //        "AES128CBC", "AES192CBC", "AES256CBC",
    //        "AES128CTR", "AES192CTR", "AES256CTR",
    //        "RANDU", "Mersenne", "MersenneAR", "VS", "C_PRG", "Rand", "Minstd", "Borland", "CMRG"
    //        "Mersenne", "MersenneAR", "VS", "C_PRG", "Rand", "Minstd", "Borland", "CMRG"
    //        "AES256CTR", "RANDU"
    //        "CHACHA-20", "SALSA-20",
            //        "Sosemanuk"
    };
    
    std::vector<functions> funs = {
            functions::T1,
            functions::T2,
            functions::T3,
            //        functions::T4,
    };
    
    for(functions fn : funs)
    {
        if(DUMP_FUNCTIONS != "")
            dump_functions(N, fn);
        else
        {
            auto ppf = select_function(N, fn);
            std::vector<statistics> stat = compute_expected_for_all(ppf, N, false);
    
            // for each starting point i
            for (int i = 1; i < N; ++i)
            {
                // compute the variance V
                rational P = trim_precision(stat[i].win_prob, 64);
                rational V = trim_precision(P * (1 - P), 64);
    
                // extract the time variance W
                rational T = trim_precision(stat[i].time_expected, 64);
                rational W = trim_precision(stat[i].time_variance, 64);
    
                // get b1 and b2 of roughly 1/100 of their corresponding expected values
                rational b1 = (P < 1_mpq / 2 ? P : 1 - P) / 100_mpq;
                rational b2 = T / 100_mpq;
    
                b1 = b1 ? b1 : 1_mpq / 100000000000_mpq; // set b1, b2 to anything if P or T
                b2 = b2 ? b2 : 1_mpq / 100000000000_mpq; // is equal to 0
                
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

                size_t max_runs = 1<<20;
                
                // if any of the runs1/runs2 would be greater than 2^20, print warning and clip runs
                if(runs1 > max_runs)
                {
                    std::cerr << "Too many runs required for i: " << i << ",\tb_1: " << b1.get_d() << ", clipping...\n";
                    runs1 = max_runs;
                    b1 = rsqrt(V / runs1) * z;
                }
                if(runs2 > max_runs)
                {
                    std::cerr << "Too many runs required for i: " << i << ",\tb_2: " << b2.get_d() << ", clipping...\n";
                    runs2 = max_runs;
                    b2 = rsqrt(W / runs2) * z;
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
    
    std::cerr << "KDF_PREFIX:\t" << KDF_PREFIX << "\n";
    std::cerr << "DUMP_FUNCTIONS:\t" << DUMP_FUNCTIONS << "\n";
    
    if(!binout)
        printf("sim;bs;i;N;p;q;won;len;\n");
    //setup_and_run_regular();
    setup_and_run_tests();
    
    for(auto it = all_tasks.begin(); it != all_tasks.end(); it = all_tasks.erase(it))
        it->wait();
}
