//
// Created by marandil on 07.01.17.
//

#include "openssl_rng.hpp"
#include "common.hpp"

#include <openssl/rc4.h>
#include <openssl/evp.h>

#include <memory>
#include <mdlutils/exceptions.hpp>
#include <iostream>

// implicitly assume there will be no cipher with block size > 1024
constexpr size_t MAX_BLOCK_SIZE = 1024;
const uint8_t zero_buffer[MAX_BLOCK_SIZE] = {}; // in C++ this shall initialize the whole array with zeroes

std::pair<size_t, uint8_t*> decode(integer value)
{
    size_t size = (mpz_sizeinbase (value.get_mpz_t(), 2) + CHAR_BIT-1) / CHAR_BIT;
    uint8_t *buffer = new uint8_t[size];
    mpz_export(buffer, nullptr, -1, size, 0, 0, value.get_mpz_t());
    return std::make_pair(size, buffer);
}

struct rc4_rng : public openssl_rng
{
    static constexpr size_t CIPHER_BLOCK_SIZE = 64;
    
    RC4_KEY rc4_key;
    
    virtual ~rc4_rng() {}
    
    rc4_rng(integer key) : openssl_rng(CIPHER_BLOCK_SIZE)
    {
        auto sb = decode(key);
        RC4_set_key(&rc4_key, int(sb.first), sb.second);
        delete[] sb.second;
    }
    
    virtual void next()
    {
        RC4(&rc4_key, CIPHER_BLOCK_SIZE, zero_buffer, this->block);
    }
};

struct evp_rng : public openssl_rng
{
    EVP_CIPHER_CTX *ctx;
    
    evp_rng(integer key, const EVP_CIPHER *type, size_t CIPHER_BLOCK_SIZE) : openssl_rng(CIPHER_BLOCK_SIZE)
    {
        ctx = EVP_CIPHER_CTX_new();
        auto sb = decode(key);
        EVP_EncryptInit_ex(ctx, type, NULL, NULL, NULL);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) <= sb.first);
        EVP_EncryptInit_ex(ctx, type, NULL, sb.second, zero_buffer);
        delete[] sb.second;
    }
    
    virtual ~evp_rng()
    {
        EVP_CIPHER_CTX_free(ctx);
    }
    
    virtual void next()
    {
        int tmp;
        EVP_EncryptUpdate(ctx, this->block, &tmp, zero_buffer, int(this->block_size));
        OPENSSL_assert(tmp == block_size);
    }
};

struct ctr_evp_rng : public openssl_rng
{
    EVP_CIPHER_CTX *ctx;
    integer counter;
    
    ctr_evp_rng(integer key, const EVP_CIPHER *type, size_t CIPHER_BLOCK_SIZE) : openssl_rng(CIPHER_BLOCK_SIZE),
                                                                                 counter(key)
    {
        ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, type, NULL, zero_buffer, zero_buffer);
    }
    
    virtual ~ctr_evp_rng()
    {
        EVP_CIPHER_CTX_free(ctx);
    }
    
    virtual void next()
    {
        int tmp;
        auto sb = decode(counter);
        EVP_EncryptUpdate(ctx, this->block, &tmp, sb.second, int(std::min(this->block_size, sb.first)));
        ++counter;
        delete[] sb.second;
        OPENSSL_assert(tmp == block_size);
    }
};

bool openssl_rng::next_bit()
{
    if(block_idx >= block_size)
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

bit_function_p openssl_rng::from_string(std::string name, integer key)
{
    if(name == "RC4")
        return std::make_shared<rc4_rng>(key);
    
    if(name == "AES128CBC")
        return std::make_shared<evp_rng>(key, EVP_aes_128_cbc(), 64);
    if(name == "AES192CBC")
        return std::make_shared<evp_rng>(key, EVP_aes_192_cbc(), 64);
    if(name == "AES256CBC")
        return std::make_shared<evp_rng>(key, EVP_aes_256_cbc(), 64);
    
    if(name == "AES128CTR")
        return std::make_shared<ctr_evp_rng>(key, EVP_aes_128_ecb(), 16);
    if(name == "AES192CTR")
        return std::make_shared<ctr_evp_rng>(key, EVP_aes_192_ecb(), 16);
    if(name == "AES256CTR")
        return std::make_shared<ctr_evp_rng>(key, EVP_aes_256_ecb(), 16);
    
    mdl_throw(mdl::not_implemented_exception, "");
}