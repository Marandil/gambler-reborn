//
// Created by marandil on 30.09.16.
//

#include "precomputed_prob_function.hpp"

#include <sstream>
#include <iostream>

precomputed_prob_function julia_prob_function(std::string fun, uint64_t N)
{
    std::string command = "julia -e \"f(i,N) = (" + fun + "); for i in 0:" + std::to_string(N) + " println(f(i," + std::to_string(N) + ")); end\"";
    std::string numbers = system_output(command);
    
    std::istringstream iss(numbers);
    
    std::vector<rational> tmp(N, 0_mpq);
    
    for(int i = 0; i < N; ++i)
    {
        std::string row;
        iss >> row;
        //std::cout << row << "\t";
        floating flo(row);
        //std::cout << flo << "\t";
        rational rat(flo);
        //std::cout << rat << "\n";
        
        tmp[i] = rat;
    }
    
    return precomputed_prob_function(tmp);
}
