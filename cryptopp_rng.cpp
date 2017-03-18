//
// Created by marandil on 15.03.17.
//

#include "cryptopp_rng.hpp"

#include <mdlutils/exceptions.hpp>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <arc4.h>
#include <salsa.h>
#include <chacha.h>
#include <sosemanuk.h>

#include <aes.h>
#include <modes.h>

#include "cipher_common.hpp"

#include <iostream>

template<typename Enc, size_t KeyLength, size_t BlockSize, typename... Args>
struct cryptopp_rng_impl : public blocked_rng
{
    Enc instance;
    
    cryptopp_rng_impl(Args... args) : blocked_rng(BlockSize), instance()
    {
    }
    
    virtual ~cryptopp_rng_impl()
    {
    }
    
    virtual void set_seed(integer seed)
    {
        auto sb = decode(seed, KeyLength);
        instance.SetKeyWithIV(sb.second, sb.first, zero_buffer);
    }
    
    virtual void next()
    {
        instance.ProcessData(this->block, zero_buffer, this->block_size);
    }
};


bit_function_p cryptopp_rng::from_string(std::string name)
{
    if (name == "RC4")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::Weak::ARC4::Encryption, 32, 32>>();
    if (name == "CHACHA-20")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::ChaCha20::Encryption, 32, 32>>();
    if (name == "SALSA-20")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::Salsa20::Encryption, 32, 32>>();
    if (name == "Sosemanuk")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::Sosemanuk::Encryption, 32, 32>>();
    
    // block ciphers
    if (name == "AES128CTR")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption, 16, 16>>();
    if (name == "AES192CTR")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption, 24, 16>>();
    if (name == "AES256CTR")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption, 32, 16>>();
    if (name == "AES128CBC")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption, 16, 16>>();
    if (name == "AES192CBC")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption, 24, 16>>();
    if (name == "AES256CBC")
        return std::make_shared<cryptopp_rng_impl<CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption, 32, 16>>();
    
    throw std::exception();
    //mdl_throw(mdl::not_implemented_exception, "");
}
