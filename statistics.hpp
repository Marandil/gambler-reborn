//
// Created by marandil on 07.11.16.
//

#ifndef GAMBLER_REBORN_STATISTICS_HPP
#define GAMBLER_REBORN_STATISTICS_HPP


#include "common.hpp"

struct statistics
{
    rational win_prob;
    rational time_expected;
    rational time_variance;
    
};

std::vector<statistics>
compute_expected_for_all(prob_function p, std::string pd, prob_function q, std::string qd, int N, bool print=false);

#endif //GAMBLER_REBORN_STATISTICS_HPP
