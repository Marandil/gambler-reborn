//
// Created by marandil on 07.01.17.
//

#include <map>
#include <mdlutils/exceptions/not_implemented_exception.hpp>
#include <mdlutils/string_utils.hpp>
#include "functions.hpp"

#include "poly_prob.hpp"

// delta - deviation from 0.5 for the upcoming tests:
std::string delta = "(1//24)";
rational delta_q = 1_mpq / 24_mpq;

//typedef std::pair<int, functions> fdef;
int fdef(int N, functions F)
{
    return N << 16 | static_cast<int>(F);
}

std::map<int, ppf> ppfs;

void init_N(int N)
{
    if (ppfs.count(fdef(N, functions::SIN)))
        return;
    
    ppfs[fdef(N, functions::ATAN)] = ppf::alloc(
            // new test: (atan(N/2 - i) / pi * delta) + 0.5
            julia_prob_function("0.5 + (atan(N / 2 - i) / pi) * 2" + delta, N),
            "atan(" + delta + ")"
    );
    
    ppfs[fdef(N, functions::SIN)] = ppf::alloc(
            // new test: (sin((i / N) * 2pi) * 2delta) + 0.5
            julia_prob_function("0.5 + (sin((i / N) * 2pi) * " + delta + ")", N),
            "sin(" + delta + ")"
    );
    
    ppfs[fdef(N, functions::ASIN)] = ppf::alloc(
            // new test: 0.5 - (asin(2x/N - 1) / pi) * delta
            julia_prob_function("0.5 - ((asin(2i/N - 1) / pi) * 2" + delta + ")", N),
            "asin(" + delta + ")"
    );
    
    ppfs[fdef(N, functions::PSIN3)] = ppf::alloc(
            // new test: 0.5 + 64 * delta * x * (-N + x) * (-N/2 + x)/(3*N**3)
            poly_prob::sinus_like3(N, delta_q),
            "poly3-sin(" + delta + ")"
    );
    
    ppfs[fdef(N, functions::PSIN5)] = ppf::alloc(
            // new test: 0.5 + x^5... - x^3... + x^1...
            poly_prob::sinus_like5(N, delta_q),
            "poly5-sin(" + delta + ")"
    );
    
    ppfs[fdef(N, functions::T1)] = ppf::alloc(
            julia_prob_function("0.48", N),
            "0.48"
    );
    
    ppfs[fdef(N, functions::T2)] = ppf::alloc(
            julia_prob_function("i/(2*i+1)" , N),
            "i/(2*i+1)"
    );
    
    ppfs[fdef(N, functions::T3)] = ppf::alloc(
            julia_prob_function("(i^3)/(i^3+(i+1)^3)" , N),
            "(i^3)/(i^3+(i+1)^3)"
    );
    
    ppfs[fdef(N, functions::T4)] = ppf::alloc(
            julia_prob_function("i/N" , N),
            "i/N"
    );
}

const ppf& select_function(int N, functions select)
{
    // plots for WolframAlpha: plot {
    //                                  0.5 + (sin((x / 128) * 2pi)/2 * 1/16),
    //                                  0.5 + (atan(128 / 2 - x) / pi) * 1/16,
    //                                  0.5 - (asin(2x/128 - 1) / pi) * 1/16
    //                              }  for x in [0, 128]
    init_N(N);
    
    switch (select)
    {
        case functions::ATAN:
        case functions::SIN:
        case functions::ASIN:
        case functions::PSIN3:
        case functions::PSIN5:
        case functions::T1:
        case functions::T2:
        case functions::T3:
        case functions::T4:
            return ppfs[fdef(N, select)];
        default:
            mdl_throw(mdl::not_implemented_exception, "");
    }
}
