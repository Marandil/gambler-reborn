//
// Created by marandil on 07.01.17.
//

#include "generators.hpp"
#include "los_rng.hpp"
#include "openssl_rng.hpp"
#include "cryptopp_rng.hpp"

#include <openssl/sha.h>

#include <mdlutils/string_utils.hpp>
#include <mdlutils/exceptions.hpp>
#include <map>
#include <thread>
#include <mutex>

integer kdf(int N, int i, uint64_t idx, uint64_t runs)
{
    std::stringstream ss;
    ss << KDF_PREFIX << "#" << N << "#" << i << "#" << idx << "#" << runs;
    std::string input = ss.str();
    unsigned char buffer[32];
    SHA256(reinterpret_cast<const unsigned char*>(input.data()), input.size(), buffer);
    std::string key = mdl::hexify(buffer, 32);
    //std::cout << "input : " << input << "\t";
    //std::cout << "KEY : " << key << "\n";
    return integer(key, 16);
}

integer kdf_related(int N, int i, uint64_t idx, uint64_t runs)
{
    std::stringstream ss;
    ss << KDF_PREFIX << "#" << N << "#" << i;
    std::string input = ss.str();
    unsigned char buffer[32];
    SHA256(reinterpret_cast<const unsigned char *>(input.data()), input.size(), buffer);
    std::string key = mdl::hexify(buffer, 32);
    
    return integer(key, 16) + idx;
}

std::map<std::thread::id, std::map<std::string, std::pair<std::string, bit_function_p>>> gen_cache;
std::mutex map_lock;

std::pair<std::string, bt_p>
select_generator(std::string generator, int N, int i, uint64_t idx, uint64_t runs)
{
    bit_function_p bf;
    std::string desc;
    {
        std::unique_lock<std::mutex> _lock(map_lock);
        auto& map = gen_cache[std::this_thread::get_id()];
        auto it = map.find(generator);
        if(it == map.end())
        {
            static std::map<std::string, std::string> los_gens = {
                    {"RANDU",      "RandU LCG"},
                    {"Mersenne",   "Mersenne Twister"},
                    {"MersenneAR", "Mersenne AR"},
                    {"VS",         "Visual Studio"},
                    {"C_PRG",      "C Rand"},
                    {"Rand",       "Old BSD"},
                    {"Minstd",     "Minstd"},
                    {"Borland",    "Borland C"},
                    {"CMRG",       "CMRG"},
            };
            static std::map<std::string, std::string> openssl_gens = {
            };
            static std::map<std::string, std::string> cryptopp_gens = {
                    {"RC4",       "RC4"},
                    {"SALSA-20",  "Salsa20"},
                    {"CHACHA-20", "ChaCha20"},
                    {"Sosemanuk", "Sosemanuk"},
                    {"AES128CBC", "AES-128 CBC Mode"},
                    {"AES192CBC", "AES-192 CBC Mode"},
                    {"AES256CBC", "AES-256 CBC Mode"},
                    {"AES128CTR", "AES-128 CTR Mode"},
                    {"AES192CTR", "AES-192 CTR Mode"},
                    {"AES256CTR", "AES-256 CTR Mode"},
            };
            
            // Handle generation of los_gens:
            if(los_gens.count(generator))
            {
                bf = los_rng::get_gen(generator.c_str());
                desc = los_gens[generator];
            }
            else if(openssl_gens.count(generator))
            {
                bf = openssl_rng::from_string(generator);
                desc = openssl_gens[generator];
            } else if (cryptopp_gens.count(generator))
            {
                bf = cryptopp_rng::from_string(generator);
                desc = cryptopp_gens[generator];
            }
            else
                mdl_throw(mdl::not_implemented_exception, "");
            map[generator] = std::make_pair(desc, bf);
        }
        else
        {
            auto pair = map[generator];
            desc = pair.first;
            bf = pair.second;
        }
    }
    integer key = kdf(N, i, idx, runs);
    bf->set_seed(key);
    return std::pair<std::string, bt_p>(desc, std::make_shared<bit_tracker::BitTracker>(bf));
}
