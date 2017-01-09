#ifndef _BIT_TRACKER_HPP
#define _BIT_TRACKER_HPP


#include "common.hpp"

#include <functional>
#include <iostream>
#include <vector>

namespace bit_tracker
{
	integer rand_range(integer range_bot, integer range_top, bit_function& get_bit);
	
	class BitTracker
    {
    protected:
        bit_function_p bit_source;
    public:
        BitTracker(bit_function_p get_bit) : bit_source(get_bit) { }
        int operator ()(const std::vector<rational>& points);
    };
}

#endif // _BIT_TRACKER_HPP
