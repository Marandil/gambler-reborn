//
// Created by marandil on 07.01.17.
//

#ifndef GAMBLER_REBORN_OPENSSL_RNG_HPP
#define GAMBLER_REBORN_OPENSSL_RNG_HPP


#include <memory>
#include <string>
#include "common.hpp"

class openssl_rng : public bit_function
{
protected:
    uint8_t* block;
    const size_t block_size;
    size_t block_idx;
    
    openssl_rng(size_t block_size) : block(new uint8_t[block_size]), block_idx(block_size), block_size(block_size) { }
public:
    virtual ~openssl_rng() { delete[] block; }
    static bit_function_p from_string(std::string name, integer key);
    
    virtual void next() = 0;
    virtual bool next_bit() override;
};

#endif //GAMBLER_REBORN_OPENSSL_RNG_HPP
