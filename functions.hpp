//
// Created by marandil on 07.01.17.
//

#ifndef GAMBLER_REBORN_FUNCTIONS_HPP
#define GAMBLER_REBORN_FUNCTIONS_HPP

#include "common.hpp"
#include "precomputed_prob_function.hpp"
#include <string>

enum class functions
{
    ATAN,
    SIN,
    ASIN,
    PSIN3,
    PSIN5,
    T1, T2, T3, T4
};

struct ppf
{
    precomputed_prob_function *p;
    precomputed_prob_function *q;
    std::string pd;
    std::string qd;
    
    //ppf() = default;
    // ppf() : p(nullptr), q(nullptr), pd(""), qd("") {}
    // ppf(const ppf& other) : p(other.p), q(other.q), pd(other.pd), qd(other.qd) { }
    
    // ppf(precomputed_prob_function *p, precomputed_prob_function *q, std::string pd, std::string qd) : p(p), q(q), pd(pd), qd(qd) { };
    
    /*
    ppf& init(precomputed_prob_function *p, precomputed_prob_function *q, std::string pd, std::string qd)
    {
        this->p = p;
        this->q = q;
        this->pd = pd;
        this->qd = qd;
        return *this;
    }
    */
    
    static ppf alloc(precomputed_prob_function p, std::string pd)
    {
        return ppf{new precomputed_prob_function(p), new precomputed_prob_function(p.get_negative()), pd, "1-" + pd};
    }
    
    static ppf alloc(precomputed_prob_function p, precomputed_prob_function q, std::string pd, std::string qd)
    {
        return ppf{
                new precomputed_prob_function(p),
                new precomputed_prob_function(q),
                pd, qd
        };
    }
};

const ppf& select_function(int, functions);

#endif //GAMBLER_REBORN_FUNCTIONS_HPP
