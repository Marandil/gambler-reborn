cmake_minimum_required(VERSION 3.6)
project(gambler_reborn)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    main.cpp
    bit_tracker.cpp bit_tracker.hpp
    common.cpp common.hpp
    gambler.hpp
    mathy.hpp
    precomputed_prob_function.cpp precomputed_prob_function.hpp
    los_rng.cpp los_rng.hpp)

add_executable(gambler_reborn ${SOURCE_FILES})
target_link_libraries(gambler_reborn gmp gmpxx Threads::Threads)