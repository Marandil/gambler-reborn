//
// Created by marandil on 30.09.16.
//

#ifndef GAMBLER_REBORN_LOS_RNG_HPP
#define GAMBLER_REBORN_LOS_RNG_HPP

namespace los_rng
{
    class PRNG
    {
    public:
        virtual void setSeed(uint64_t seed) = 0;
        
        virtual uint64_t nextInt() = 0;
        
        virtual uint32_t getNrOfBits() = 0;
    };
    
    std::shared_ptr<PRNG> getPRNG(const char *name);
    
    class BitSource
    {
        std::shared_ptr<PRNG> rng;
        size_t curr;
        size_t bits;
        size_t word;
    public:
        BitSource(std::shared_ptr<PRNG> rng) : rng(rng), bits(rng->getNrOfBits()), curr(0), word(0) { }
        
        bool operator()()
        {
            if(!curr)
            {
                word = rng->nextInt();
                curr = bits;
            }
            bool r = word & 1;
            word >>= 1;
            --bits;
            return r;
        }
    };
}


#endif //GAMBLER_REBORN_LOS_RNG_HPP
