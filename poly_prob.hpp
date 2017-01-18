//
// Created by marandil on 30.10.16.
//

#ifndef GAMBLER_REBORN_POLY_PROB_HPP
#define GAMBLER_REBORN_POLY_PROB_HPP


#include "precomputed_prob_function.hpp"

namespace poly_prob
{
    // 64*d*x*(-N + x)*(-N/2 + x)/(3*N**3) + 1/2
    precomputed_prob_function_p sinus_like3(int N, rational delta);
    
    // ((16 * x^5)/(9 * pi^5) - (44 * x^3)/(9 * pi^3) + (28 * x)/(9*pi))
    precomputed_prob_function_p sinus_like5(int N, rational delta);
}

#endif //GAMBLER_REBORN_POLY_PROB_HPP
