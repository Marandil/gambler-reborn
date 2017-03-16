//
// Created by marandil on 07.01.17.
//

#include "openssl_rng.hpp"

#include <openssl/rc4.h>
#include <openssl/evp.h>

#include <mdlutils/exceptions.hpp>
#include <iostream>

#include "cipher_common.hpp"

struct rc4_rng : public blocked_rng
{
    static constexpr size_t CIPHER_BLOCK_SIZE = 64;
    
    RC4_KEY rc4_key;
    
    virtual ~rc4_rng() {}
    
    rc4_rng() : blocked_rng(CIPHER_BLOCK_SIZE)
    {
    }
    
    virtual void set_seed(integer key)
    {
        auto sb = decode(key, 32);
        RC4_set_key(&rc4_key, int(sb.first), sb.second);
        delete[] sb.second;
    }
    
    virtual void next()
    {
        RC4(&rc4_key, CIPHER_BLOCK_SIZE, zero_buffer, this->block);
    }
};

struct evp_rng : public blocked_rng
{
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER *type;
    
    evp_rng(const EVP_CIPHER *type, size_t CIPHER_BLOCK_SIZE) : blocked_rng(CIPHER_BLOCK_SIZE), type(type)
    {
        ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, type, NULL, NULL, NULL);
    }
    
    virtual ~evp_rng()
    {
        EVP_CIPHER_CTX_free(ctx);
    }
    
    virtual void set_seed(integer key)
    {
        auto sb = decode(key, 32);
        OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) <= sb.first);
        EVP_EncryptInit_ex(ctx, type, NULL, sb.second, zero_buffer);
        delete[] sb.second;
    }
    
    virtual void next()
    {
        int tmp;
        EVP_EncryptUpdate(ctx, this->block, &tmp, zero_buffer, int(this->block_size));
        OPENSSL_assert(tmp == block_size);
    }
};

struct ctr_evp_rng : public blocked_rng
{
    EVP_CIPHER_CTX *ctx;
    integer counter;
    
    ctr_evp_rng(const EVP_CIPHER *type, size_t CIPHER_BLOCK_SIZE) : blocked_rng(CIPHER_BLOCK_SIZE),
                                                                    counter(0)
    {
        ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, type, NULL, zero_buffer, zero_buffer);
    }
    
    virtual ~ctr_evp_rng()
    {
        EVP_CIPHER_CTX_free(ctx);
    }
    
    virtual void set_seed(integer seed)
    {
        counter = seed;
    }
    
    virtual void next()
    {
        int tmp;
        auto sb = decode(counter, 32);
        EVP_EncryptUpdate(ctx, this->block, &tmp, sb.second, int(std::min(this->block_size, sb.first)));
        ++counter;
        delete[] sb.second;
        OPENSSL_assert(tmp == block_size);
    }
};

bit_function_p openssl_rng::from_string(std::string name)
{
    if(name == "RC4")
        return std::make_shared<rc4_rng>();
    
    if(name == "AES128CBC")
        return std::make_shared<evp_rng>(EVP_aes_128_cbc(), 64);
    if(name == "AES192CBC")
        return std::make_shared<evp_rng>(EVP_aes_192_cbc(), 64);
    if(name == "AES256CBC")
        return std::make_shared<evp_rng>(EVP_aes_256_cbc(), 64);
    
    if(name == "AES128CTR")
        return std::make_shared<ctr_evp_rng>(EVP_aes_128_ecb(), 16);
    if(name == "AES192CTR")
        return std::make_shared<ctr_evp_rng>(EVP_aes_192_ecb(), 16);
    if(name == "AES256CTR")
        return std::make_shared<ctr_evp_rng>(EVP_aes_256_ecb(), 16);
    
    mdl_throw(mdl::not_implemented_exception, "");
}
