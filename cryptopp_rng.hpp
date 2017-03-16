//
// Created by marandil on 15.03.17.
//

#ifndef GAMBLER_REBORN_CRYPTOPP_RNG_HPP
#define GAMBLER_REBORN_CRYPTOPP_RNG_HPP


#include <memory>
#include <string>
#include "common.hpp"

namespace cryptopp_rng
{
    bit_function_p from_string(std::string name);
};

#endif //GAMBLER_REBORN_CRYPTOPP_RNG_HPP
