#ifndef _COMMON_HPP
#define _COMMON_HPP

#include <gmpxx.h>

#include <cstdint>
#include <functional>
#include <vector>
#include <memory>

typedef mpz_class integer;
typedef mpq_class rational;
typedef mpf_class floating;

typedef std::function<rational(uint64_t,uint64_t)> prob_function;
//typedef std::function<bool()> bit_function;
typedef std::function<int(const std::vector<rational>& points)> sim_function;

struct bit_function {
    virtual ~bit_function() { };
    virtual bool operator()() = 0;
};
typedef std::shared_ptr<bit_function> bit_function_p;

std::string system_output(std::string cmd);

rational parse_rational(std::string rat);
rational parse_floating_rational(std::string flt);
rational trim_precision(rational, ptrdiff_t size);

extern std::string KDF_PREFIX;

#endif // _COMMON_HPP
