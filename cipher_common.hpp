//
// Created by marandil on 15.03.17.
//

#ifndef GAMBLER_REBORN_CIPHER_COMMON_HPP
#define GAMBLER_REBORN_CIPHER_COMMON_HPP

#include "common.hpp"

// implicitly assume there will be no cipher with block size > 1024
constexpr size_t MAX_BLOCK_SIZE = 1024;
extern const uint8_t zero_buffer[];

std::pair<size_t, uint8_t *> decode(integer value, size_t size = 0);


class blocked_rng : public bit_function
{
protected:
    uint8_t *block;
    const size_t block_size;
    size_t block_idx;
    
    blocked_rng(size_t block_size) : block(new uint8_t[block_size]), block_idx(block_size), block_size(block_size) {}

public:
    virtual ~blocked_rng() { delete[] block; }
    
    virtual void next() = 0;
    
    virtual bool next_bit() override;
};


#endif //GAMBLER_REBORN_CIPHER_COMMON_HPP
