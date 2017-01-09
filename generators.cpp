//
// Created by marandil on 07.01.17.
//

#include "generators.hpp"
#include "bit_tracker.hpp"
#include "los_rng.hpp"
#include "openssl_rng.hpp"

#include <openssl/sha.h>
#include <openssl/rc4.h>

#include <mdlutils/string_utils.hpp>
#include <map>
#include <mdlutils/exceptions.hpp>

integer kdf(int N, int i, uint64_t idx, uint64_t runs)
{
    std::string input = KDF_PREFIX + "#" + std::to_string(idx + (i * runs) + 1); // +1 to match Julia indices
    unsigned char buffer[32];
    SHA256(reinterpret_cast<const unsigned char*>(input.data()), input.size(), buffer);
    std::string key = mdl::hexify(buffer, 32);
    //std::cout << "input : " << input << "\t";
    //std::cout << "KEY : " << key << "\n";
    return integer(key, 16);
}


std::pair<std::string, bt_p>
select_generator(std::string generator, int N, int i, uint64_t idx, uint64_t runs)
{
    static std::map<std::string, std::string> los_gens = {
            {"RANDU", "RandU LCG"},
            {"Mersenne", "Mersenne Twister"},
            {"MersenneAR", "Mersenne AR"},
            {"VS", "Visual Studio"},
            {"C_PRG", "C Rand"},
            {"Rand", "Old BSD"},
            {"Minstd", "Minstd"},
            {"Borland", "Borland C"},
            {"CMRG", "CMRG"}
    };
    static std::map<std::string, std::string> openssl_gens = {
            {"RC4", "RC4"},
            {"AES128CBC", "AES-128 CBC Mode"},
            {"AES192CBC", "AES-192 CBC Mode"},
            {"AES256CBC", "AES-256 CBC Mode"},
            {"AES128CTR", "AES-128 CTR Mode"},
            {"AES192CTR", "AES-192 CTR Mode"},
            {"AES256CTR", "AES-256 CTR Mode"}
    };
    
    // Handle generation of los_gens:
    if(los_gens.count(generator))
    {
        integer key = kdf(N, i, idx, runs);
        bit_function_p bf = los_rng::get_gen(generator.c_str())(key);
        return std::make_pair(los_gens[generator], std::make_shared<bit_tracker::BitTracker>(bf));
    }
    if(openssl_gens.count(generator))
    {
        integer key = kdf(N, i, idx, runs);
        bit_function_p bf = openssl_rng::from_string(generator, key);
        return std::make_pair(openssl_gens[generator], std::make_shared<bit_tracker::BitTracker>(bf));
    }
    mdl_throw(mdl::not_implemented_exception, "");
}
