//
// Created by marandil on 30.09.16.
//

#ifndef GAMBLER_REBORN_LOS_RNG_HPP
#define GAMBLER_REBORN_LOS_RNG_HPP

#include <iostream>
#include "common.hpp"

namespace los_rng
{
    class PRNG
    {
    public:
        virtual void setSeed(integer seed) = 0;
        
        virtual uint64_t nextInt() = 0;
        
        virtual uint32_t getNrOfBits() = 0;
    };
    
    std::shared_ptr<PRNG> getPRNG(const char *name);
    void generateString(uint64_t nrOfBits, std::shared_ptr<PRNG> prng);
    
    class BitSource : public bit_function
    {
        std::shared_ptr<PRNG> rng;
        size_t curr;
        size_t bits;
        size_t word;
    public:
        BitSource(std::shared_ptr<PRNG> rng) : rng(rng), bits(rng->getNrOfBits()), curr(0), word(0) { }
        
        virtual bool next_bit() override
        {
            if(!curr)
            {
                word = rng->nextInt();
                curr = bits;
            }
            bool r = word & 1;
            word >>= 1;
            --curr;
            //std::cout << " < " << r << "\t" << word << "\t" << curr << "\n";
            return r;
        }
    };
    
    inline std::function<bit_function_p(integer)> get_gen(const char* name)
    {
        return [=](integer seed)
            {
                auto rng = los_rng::getPRNG(name);
                rng->setSeed(seed);
                return std::make_shared<los_rng::BitSource>(rng);
            };
    }
}


#endif //GAMBLER_REBORN_LOS_RNG_HPP
