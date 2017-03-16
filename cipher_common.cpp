//
// Created by marandil on 15.03.17.
//

#include "cipher_common.hpp"

#include <filters.h>
#include <hex.h>

#include <iostream>

const uint8_t zero_buffer[MAX_BLOCK_SIZE] = {}; // in C++ this shall initialize the whole array with zeroes

std::pair<size_t, uint8_t *> decode(integer value, size_t size)
{
    /*
    if(!size)
        size = (mpz_sizeinbase (value.get_mpz_t(), 2) + CHAR_BIT-1) / CHAR_BIT;
    uint8_t *buffer = new uint8_t[size];
    mpz_export(buffer, nullptr, -1, size, 0, 0, value.get_mpz_t());
    return std::make_pair(size, buffer);
    */
    
    std::string hex = value.get_str(16);
    if (!size) size = (hex.size() + 1) / 2;
    
    uint8_t *array = new uint8_t[size];
    
    while (hex.size() < 2 * size)
        hex = "0" + hex;
    
    size_t offs = hex.size() - 2 * size;
    hex = hex.substr(offs);
    CryptoPP::StringSource(hex, true,
                           new CryptoPP::HexDecoder(new CryptoPP::ArraySink(array, size)));
    
    return std::make_pair(size, array);
}

bool blocked_rng::next_bit()
{
    if (block_idx >= block_size)
    {
        this->next();
        block_idx = 0;
    }
    size_t byte_idx = block_idx / 8;
    size_t bit_idx = block_idx % 8;
    ++block_idx;
    //std::cout << "..." << int(block[byte_idx]) << "..." << bool((block[byte_idx] >> bit_idx) & 1) << "..." << block_idx << "\n";
    return bool((block[byte_idx] >> bit_idx) & 1);
}

