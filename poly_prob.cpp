//
// Created by marandil on 30.10.16.
//

#include "poly_prob.hpp"

namespace poly_prob
{
    // 64*d*x*(-N + x)*(-N/2 + x)/(3*N**3)
    precomputed_prob_function_p sinus_like3(int N, rational delta)
    {
        const rational rN = rational(N);
        const rational denom = (3 * rN * rN * rN);
        
        auto func = [delta, rN, denom](int x, int _)
            {
                const rational nom = 64 * delta * x * (-rN + x) * (-rN / 2 + x);
                const rational result = 1_mpq / 2_mpq + nom / denom;
                return result;
            };
        return std::make_shared<precomputed_prob_function>(func, N);
    }
    
    // ((16 * (x-N/2)^5)/(9 * N^5) - (44 * (x-N/2)^3)/(9 * N^3) + (28 * (x-N/2))/(9*N)) * delta + 1/2
    precomputed_prob_function_p sinus_like5(int N, rational delta)
    {
        const rational rN = N;
        const rational rNN = rN*rN;
        const rational rNNN = rN*rNN;
        const rational rNNNNN = rNNN * rNN;
        
        auto func = [delta, rN, rNNN, rNNNNN](int x, int _)
            {
                const rational X = (x - rN / 2_mpq);
                const rational XXX = X*X*X;
                const rational XXXXX = X*XXX*X;
                const rational x1 = (28_mpq * X) / (9_mpq * rN);
                const rational x3 = (44_mpq * XXX) / (9_mpq * rNNN);
                const rational x5 = (16_mpq * XXXXX) / (9_mpq * rNNNNN);
                const rational result = 1_mpq / 2_mpq - (x1 - x3 + x5) * delta;
                return result;
            };
        return std::make_shared<precomputed_prob_function>(func, N);
    }
}
