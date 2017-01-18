//
// Created by marandil on 07.01.17.
//

#ifndef GAMBLER_REBORN_FUNCTIONS_HPP
#define GAMBLER_REBORN_FUNCTIONS_HPP

#include "common.hpp"
#include "precomputed_prob_function.hpp"
#include <string>

enum class functions
{
    ATAN,
    SIN,
    ASIN,
    PSIN3,
    PSIN5,
    T1, T2, T3, T4
};

struct ppf
{
    precomputed_prob_function_p p;
    precomputed_prob_function_p q;
    std::string pd;
    std::string qd;
    
    static ppf anew(precomputed_prob_function_p p, std::string pd)
    {
        return ppf{p, p->get_negative(), pd, "1-" + pd};
    }
    
    static ppf anew(precomputed_prob_function_p p, precomputed_prob_function_p q, std::string pd, std::string qd)
    {
        return ppf{
                p, q, pd, qd
        };
    }
};


ppf select_function(int, functions);

#endif //GAMBLER_REBORN_FUNCTIONS_HPP
