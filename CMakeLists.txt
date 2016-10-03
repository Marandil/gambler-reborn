cmake_minimum_required(VERSION 2.8)
project(gambler_reborn)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# mdlutils-cpp
add_subdirectory(mdlutils-cpp)
include_directories(mdlutils-cpp/include)

set(SOURCE_FILES
    main.cpp
    bit_tracker.cpp bit_tracker.hpp
    common.cpp common.hpp
    gambler.hpp
    mathy.hpp
    precomputed_prob_function.cpp precomputed_prob_function.hpp
    los_rng.cpp los_rng.hpp mt19937ar.cpp mt19937ar.hpp)

add_executable(gambler_reborn ${SOURCE_FILES})
target_link_libraries(gambler_reborn gmp gmpxx Threads::Threads mdlutils crypto)