#ifndef _GAMBLER_HPP
#define _GAMBLER_HPP


#include "common.hpp"
#include "functions.hpp"

#include <functional>
#include <iostream>

namespace gambler
{
    class Gambler1D
    {
        uint64_t time;
        
        uint64_t value;
        uint64_t limit;
    
        precomputed_prob_function_p p, q; // step win/loss probability function
        
        uint64_t step_win;
        uint64_t step_none;
        uint64_t step_loss;
    
    public:
        Gambler1D(uint64_t start, uint64_t limit, precomputed_prob_function_p p, precomputed_prob_function_p q,
                  uint64_t stepWin = 1, uint64_t stepLoss = -1, uint64_t stepNone = 0)
                : value(start), limit(limit), time(0), p(p), q(q),
                  step_win(stepWin), step_none(stepNone), step_loss(stepLoss) {}
        
        bool step_regular(sim_function& random)
        {
            rational _p = (*p)(value, limit);
            rational _q = (*q)(value, limit);
            
            int outcome = random({_p, _p + _q});
            switch (outcome)
            {
                case 0:
                    value += step_win;
                    break;
                case 1:
                    value += step_loss;
                    break;
                default:
                    value += step_none;
                    break;
            }
            time += 1;
            return !is_finished();
        }
    
        bool is_won() const { return value == limit; }
        bool is_lost() const { return value == 0; }
        bool is_finished() const { return is_won() || is_lost(); }
        
        std::pair<bool, uint64_t> run_gambler(sim_function random)
        {
            //std::cout << "TIME S : " << time << "\n";
            while(!is_finished())
                step_regular(random);
            //std::cout << "TIME E : " << time << "\n";
            return std::make_pair(is_won(), time);
        };
    };
}

#endif // _GAMBLER_HPP
