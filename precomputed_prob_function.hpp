//
// Created by marandil on 30.09.16.
//

#ifndef GAMBLER_REBORN_PRECOMPUTED_PROB_FUNCTION_HPP
#define GAMBLER_REBORN_PRECOMPUTED_PROB_FUNCTION_HPP

#include <iostream>
#include "common.hpp"

class precomputed_prob_function;
typedef std::shared_ptr<precomputed_prob_function> precomputed_prob_function_p;

class precomputed_prob_function
{
protected:
    std::vector<rational> buffer;
    
    void print_stats() const
    {
        const size_t N = buffer.size();
        rational min = 1, max = 0, sum = 0;
        size_t min_bits = 1 << 30, max_bits = 0, sum_bits = 0;
    
        for(int i = 1; i < N; ++i)
        {
            if(buffer[i] < min)
                min = buffer[i];
            if(buffer[i] > max)
                max = buffer[i];
            sum += buffer[i];
    
            size_t bits = mpz_sizeinbase(buffer[i].get_num_mpz_t(), 2) + mpz_sizeinbase(buffer[i].get_den_mpz_t(), 2);
    
            if (bits < min_bits)
                min_bits = bits;
            if (bits > max_bits)
                max_bits = bits;
            sum += bits;
        }
        
        double d_min = min.get_d();
        double d_max = max.get_d();
        double d_sum = sum.get_d();
    
        //std::cout << " Precomputed prob function; min : " << d_min << ", max : " << d_max << ", avg : " << (d_sum / (N-1)) << "\n";
        //std::cout << " Precomputed prob function; min bits : " << min_bits << ", max bits : " << max_bits << ", avg : " << (sum_bits / (N-1)) << "\n";
    }
    
public:
    precomputed_prob_function(prob_function base, int N) : buffer(N, 0_mpq)
    {
        for(int i = 0; i < N; ++i)
            buffer[i] = base(i, N);
    
        print_stats();
    }
    
    precomputed_prob_function(const std::vector<rational>& pattern) : buffer(pattern)
    {
        print_stats();
    }
    
    precomputed_prob_function(const precomputed_prob_function &) = delete;
    
    rational operator ()(uint64_t i, uint64_t N) const { return buffer[i]; }
    
    precomputed_prob_function_p get_negative() const;
    
    void dump(void) const
    {
        for(int i=0; i < buffer.size(); ++i)
            std::cout << i << "\t" << buffer[i] << "\n";
    }
};

precomputed_prob_function_p julia_prob_function(std::string fun, uint64_t N);

#endif //GAMBLER_REBORN_PRECOMPUTED_PROB_FUNCTION_HPP
