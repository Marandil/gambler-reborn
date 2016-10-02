//
// Created by marandil on 30.09.16.
//

#include <cstdio>
#include <string>
#include <fstream>
#include <streambuf>
#include <gmpxx.h>
#include <stdlib.h>
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

rational parse_floating_rational(std::string flt)
{
    size_t pt = flt.find('.');
    std::string dec = flt.substr(0, pt-1);
    std::string frac = flt.substr(pt+1);
    
    size_t div_power = frac.length();
    
    integer nom(dec + frac);
    integer den = 10;
    mpz_pow_ui(den.get_mpz_t(), den.get_mpz_t(), div_power);
    
    return rational(nom, den);
}
