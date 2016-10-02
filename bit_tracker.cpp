//
// Created by marandil on 30.09.16.
//

#include <cassert>
#include "bit_tracker.hpp"

namespace bit_tracker
{
    integer rand_range(integer range_bot, integer range_top, bit_function get_bit)
    {
        integer k = range_top - range_bot;
        
        // start with l = 0, r = 1
        // and scale from [0, 1) to [0, k)
        rational l = 0, r = k;
        
        // first, go down with bit-tracker algorithm, until r-l <= 1/k (scaled: 1)
        while(r - l > 1)
        {
            if(get_bit())	// on 1 go right
                l = (r + l) / 2;
            else			// on 0 go left
                r = (r + l) / 2;
        }
        
        integer l_int, r_int;
        
        while(true)
        {
            // from now on, check if [l,r) is between i/k and (i+1)/k (scaled: i, i+1)
            // for l get floor:
            l_int = l.get_num() / l.get_den();
            // for r get ceil:
            r_int = (l.get_num() + l.get_den() - 1) / l.get_den();
            
            // l_int is the left boundary  [ of the interval of l [i, i+1)
            // r_int is the right boundary ] of the interval of r (j, j+1]
            
            // if (r_int - l_int) == 1 then i == j and we have an interval
            if((r_int - l_int) <= 1)
                break;
            
            if(get_bit())	// on 1 go right
                l = (r + l) / 2;
            else			// on 0 go left
                r = (r + l) / 2;
        }
        
        return l_int + range_bot;
    }
    
    int check_in_range(const std::vector<rational>& points, const rational& value)
    {
        int idx = 0;
        for(const rational& point : points)
        {
            if (value < point)
                return idx;
            else
                ++idx;
        }
        return idx;
    }
    
    int check_in_range_rev(const std::vector<rational>& points, const rational& value)
    {
        int idx = 0;
        for(const rational& point : points)
        {
            if (value <= point)
                return idx;
            else
                ++idx;
        }
        return idx;
    }
    
    int BitTracker::operator()(const std::vector<rational>& points)
    {
        static int i = 0;
        rational l = 0, r = 1;
        //if(i >= 0 && i <= 8) std::cout << "\nF: " << points[0] << " | " << points[1] << "\n";
    
        int range_l = 0, range_r = check_in_range_rev(points, r);;
        while(range_l != range_r)
        {
            bool bit = this->bit_source();
            if (bit)            // on 1 go right
            {
                l = (r + l) / 2;
                range_l = check_in_range(points, l);
            }
            else            // on 0 go left
            {
                r = (r + l) / 2;
                range_r = check_in_range_rev(points, r);
            }
            //if(i >= 0 && i <= 8) std::cout << l << "(" << range_l << ") " << r << "(" << range_r << "), [" << bit << "]\n";
            //assert(++i != 20);
        }
        //std::cout << "[" << range_l << "]" << (++i % 20 == 0 ? "\n" : "");
        assert(range_l != 2);
        return range_l;
    }
}