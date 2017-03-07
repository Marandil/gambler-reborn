//
// Created by marandil on 30.09.16.
//

#ifndef GAMBLER_REBORN_LOS_RNG_HPP
#define GAMBLER_REBORN_LOS_RNG_HPP

#include <iostream>
#include "common.hpp"

namespace los_rng
{
    class PRNG : public bit_function
    {
    protected:
        size_t curr;
        size_t word;

        PRNG(size_t bits) : bits(bits), curr(0), word(0) { }
        
    public:
        size_t bits;
        
        virtual bool next_bit()
        {
            if(!curr)
            {
                word = this->nextInt();
                curr = bits;
            }
            bool r = word & 1;
            word >>= 1;
            --curr;
            //std::cout << " < " << r << "\t" << word << "\t" << curr << "\n";
            return r;
        }
        
        virtual void set_seed(integer seed) = 0;
        
        virtual uint64_t nextInt() = 0;
    };
    
    std::shared_ptr<PRNG> getPRNG(const char *name);
    void generateString(uint64_t nrOfBits, std::shared_ptr<PRNG> prng);
    
    inline bit_function_p get_gen(const char* name)
    {
        auto rng = los_rng::getPRNG(name);
        return rng;
    }
}


#endif //GAMBLER_REBORN_LOS_RNG_HPP
