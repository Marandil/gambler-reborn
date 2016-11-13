//
// Created by marandil on 30.10.16.
//

#include "poly_prob.hpp"

namespace poly_prob
{
    // 64*d*x*(-N + x)*(-N/2 + x)/(3*N**3)
    precomputed_prob_function sinus_like3(int N, rational delta)
    {
        auto func = [delta](int x, int N)
            {
                const rational denom = (3 * N * N * N);
                const rational nom = 64 * delta * x * (-N + x) * (-N / 2 + x);
                const rational result = 1_mpq / 2_mpq + nom / denom;
                return result;
            };
        return precomputed_prob_function(func, N);
    }
    
    // ((16 * (x-N/2)^5)/(9 * N^5) - (44 * (x-N/2)^3)/(9 * N^3) + (28 * (x-N/2))/(9*N)) * delta + 1/2
    precomputed_prob_function sinus_like5(int N, rational delta)
    {
        auto func = [delta](int x, int N)
            {
                const rational X = (x - N / 2_mpq);
                const rational XXX = X*X*X;
                const rational XXXXX = X*XXX*X;
                const rational x1 = (28_mpq * X) / (9_mpq * N);
                const rational x3 = (44_mpq * XXX) / (9_mpq * N*N*N);
                const rational x5 = (16_mpq * XXXXX) / (9_mpq * N*N*N*N*N);
                const rational result = 1_mpq / 2_mpq - (x1 - x3 + x5) * delta;
                return result;
            };
        return precomputed_prob_function(func, N);
    }
}
