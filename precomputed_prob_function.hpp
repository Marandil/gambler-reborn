//
// Created by marandil on 30.09.16.
//

#ifndef GAMBLER_REBORN_PRECOMPUTED_PROB_FUNCTION_HPP
#define GAMBLER_REBORN_PRECOMPUTED_PROB_FUNCTION_HPP

#include <iostream>
#include "common.hpp"

class precomputed_prob_function
{
protected:
    std::vector<rational> buffer;
    
public:
    precomputed_prob_function(prob_function base, int N) : buffer(N, 0_mpq)
    {
        for(int i = 0; i < N; ++i)
            buffer[i] = base(i, N);
    }
    
    precomputed_prob_function(const std::vector<rational>& pattern) : buffer(pattern) { }
    
    rational operator ()(uint64_t i, uint64_t N) const { return buffer[i]; }
    
    precomputed_prob_function get_negative() const;
    
    void dump(void) const
    {
        for(int i=0; i < buffer.size(); ++i)
            std::cout << i << "\t" << buffer[i] << "\n";
    }
};

precomputed_prob_function julia_prob_function(std::string fun, uint64_t N);

#endif //GAMBLER_REBORN_PRECOMPUTED_PROB_FUNCTION_HPP
