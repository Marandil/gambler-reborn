//
// Created by marandil on 30.09.16.
//

#include <cstdio>
#include <string>
#include <fstream>
#include <streambuf>
#include <gmpxx.h>
#include <stdlib.h>
#include <iostream>
#include "common.hpp"

std::string system_output(std::string cmd)
{
    std::string tmpfile = tmpnam(nullptr);
    cmd = cmd + " > " + tmpfile;
    system(cmd.c_str());
    std::ifstream t(tmpfile);
    std::string buffer;
    
    t.seekg(0, std::ios::end);
    buffer.reserve(t.tellg());
    t.seekg(0, std::ios::beg);
    
    buffer.assign((std::istreambuf_iterator<char>(t)),
                  std::istreambuf_iterator<char>());
    
    return buffer;
}

rational parse_rational(std::string rat)
{
    size_t pt = rat.find("//");
    std::string snom = rat.substr(0, pt);
    std::string sden = rat.substr(pt+2);
    
    //std::cerr << snom << " // " << sden << "\n";
    
    integer nom(snom);
    integer den(sden);
    
    return rational(nom, den);
}

rational parse_floating_rational(std::string flt)
{
    size_t pt = flt.find('.');
    std::string dec = flt.substr(0, pt);
    std::string frac = flt.substr(pt+1);
    
    size_t div_power = frac.length();
    
    integer nom(dec + frac);
    integer den = 10;
    mpz_pow_ui(den.get_mpz_t(), den.get_mpz_t(), div_power);
    
    return rational(nom, den);
}

rational trim_precision(rational r, ptrdiff_t size)
{
    ptrdiff_t num_bits = mpz_sizeinbase (r.get_num_mpz_t(), 2);
    ptrdiff_t den_bits = mpz_sizeinbase (r.get_den_mpz_t(), 2);
    ptrdiff_t diff = den_bits - num_bits;
    
    // std::cout << "[D " << diff << "]\t";
    
    // if after trimming, denominator would be size larger than num, return 0
    if(diff > size)
        return 0_mpq;
    
    // count the number of bits we need to trim for den:
    ptrdiff_t trim = den_bits - size;
    
    // std::cout << "[T " << trim << "]\t";
    // if the trim is < 0, return r, no trimming
    if(trim < 0)
        return r;
    
    // extract num and den and shift them right by trim:
    integer num = r.get_num() >> trim;
    integer den = r.get_den() >> trim;
    
    // return a new, trimmed rational
    return rational(num, den);
}

rational rsqrt(rational op)
{
    // Convert to multiprecision floating point
    mpf_class op_f(op);
    mpf_class root = sqrt(op_f);
    return rational(root);
}
