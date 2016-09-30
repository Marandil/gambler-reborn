#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <gmpxx.h>

#include <cstdint>
#include <functional>
#include <vector>

typedef mpz_class integer;
typedef mpq_class rational;
typedef mpf_class floating;

typedef std::function<rational(uint64_t,uint64_t)> prob_function;
typedef std::function<bool()> bit_function;
typedef std::function<int(const std::vector<rational>& points)> sim_function;

std::string system_output(std::string cmd);
	
#endif // _COMMON_HPP
