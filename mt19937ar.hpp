//
// Created by marandil on 01.10.16.
//

#ifndef GAMBLER_REBORN_MT19937AR_HPP
#define GAMBLER_REBORN_MT19937AR_HPP


#include "common.hpp"

class mt19937ar
{

/* Period parameters */
    static constexpr size_t N = 624;
    static constexpr size_t M = 397;
    static constexpr size_t MATRIX_A = 0x9908b0dfUL;   /* constant vector a */
    static constexpr size_t UPPER_MASK = 0x80000000UL; /* most significant w-r bits */
    static constexpr size_t LOWER_MASK = 0x7fffffffUL; /* least significant r bits */
    
    unsigned long mt[N]; /* the array for the state vector  */
    int mti = N + 1; /* mti==N+1 means mt[N] is not initialized */

public:
    mt19937ar(unsigned long s);
    
    mt19937ar(unsigned long init_key[], int key_length);
    
    void srand(unsigned long s);
    
    unsigned long genrand_int32(void);
};


#endif //GAMBLER_REBORN_MT19937AR_HPP
