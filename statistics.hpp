//
// Created by marandil on 07.11.16.
//

#ifndef GAMBLER_REBORN_STATISTICS_HPP
#define GAMBLER_REBORN_STATISTICS_HPP


#include "common.hpp"
#include "functions.hpp"

struct statistics
{
    rational win_prob;
    rational time_expected;
    rational time_variance;
    
};

std::vector<statistics>
compute_expected_for_all(ppf pq, int N, bool print = false);

#endif //GAMBLER_REBORN_STATISTICS_HPP
