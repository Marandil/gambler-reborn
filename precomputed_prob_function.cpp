//
// Created by marandil on 30.09.16.
//

#include "precomputed_prob_function.hpp"

#include <sstream>
#include <iostream>

precomputed_prob_function_p julia_prob_function(std::string fun, uint64_t N)
{
    std::string command = "julia -e \"f(i,N) = (" + fun + "); for i in 0:" + std::to_string(N) + " println(Rational(f(i," + std::to_string(N) + "))); end\"";
    std::string numbers = system_output(command);
    
    std::istringstream iss(numbers);
    
    std::vector<rational> tmp(N, 0_mpq);
    
    for(int i = 0; i < N; ++i)
    {
        std::string row;
        iss >> row;
        //std::cout << row << "\t";
        //floating flo(row);
        //std::cout << flo << "\t";
        //rational rat(flo);
        //std::cout << rat << "\n";
        
        rational rat = parse_rational(row);
        
        tmp[i] = rat;
    }
    
    return std::make_shared<precomputed_prob_function>(tmp);
}

precomputed_prob_function_p precomputed_prob_function::get_negative() const
{
    std::vector<rational> tmp(this->buffer);
    for(auto& value : tmp)
        value = 1 - value;
    return std::make_shared<precomputed_prob_function>(tmp);
}
