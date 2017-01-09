//
// Created by marandil on 07.01.17.
//

#ifndef GAMBLER_REBORN_GENERATORS_HPP
#define GAMBLER_REBORN_GENERATORS_HPP

#include <string>
#include <memory>
#include "common.hpp"
#include "bit_tracker.hpp"

typedef std::shared_ptr<bit_tracker::BitTracker> bt_p;

// Use the middle parameters to create and initialize generator `generator`
std::pair<std::string, bt_p> select_generator(std::string generator, int N, int i, uint64_t idx, uint64_t runs);

#endif //GAMBLER_REBORN_GENERATORS_HPP
