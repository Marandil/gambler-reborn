cmake_minimum_required(VERSION 2.8)
project(gambler_reborn)

find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# mdlutils-cpp
add_subdirectory(mdlutils-cpp)
include_directories(mdlutils-cpp/include)

# cryptopp
add_subdirectory(cryptopp)
include_directories(cryptopp)

set(SOURCE_FILES
    main.cpp
    bit_tracker.cpp bit_tracker.hpp
    common.cpp common.hpp
    gambler.hpp
    precomputed_prob_function.cpp precomputed_prob_function.hpp
    los_rng.cpp los_rng.hpp
        mt19937ar.cpp mt19937ar.hpp poly_prob.cpp poly_prob.hpp run_config.cpp run_config.hpp statistics.cpp statistics.hpp functions.cpp functions.hpp generators.cpp generators.hpp openssl_rng.cpp openssl_rng.hpp cryptopp_rng.cpp cryptopp_rng.hpp cipher_common.cpp cipher_common.hpp)

add_executable(gambler_reborn ${SOURCE_FILES})
target_link_libraries(gambler_reborn gmp gmpxx)
target_link_libraries(gambler_reborn ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(gambler_reborn mdlutils)
target_link_libraries(gambler_reborn crypto)
target_link_libraries(gambler_reborn cryptopp-static)

add_executable(ppf_test ppf.cpp)
