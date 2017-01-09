//
// Created by marandil on 07.11.16.
//

#include <iostream>
#include "statistics.hpp"
#include <limits>

typedef std::numeric_limits< double > dbl;

template<typename function>
inline rational sum(int from, int to, const function& lambda)
{
    rational result = 0_mpq;
    for(int i = from; i <= to; ++i)
    {
        result += lambda(i);
    }
    return result;
}

template<>
inline rational sum(int from, int to, const std::vector<rational>& vector)
{
    rational result = 0_mpq;
    for(int i = from; i <= to; ++i)
        result += vector[i];
    return result;
}

template<typename function>
inline rational prod(int from, int to, const function& lambda)
{
    rational result = 1_mpq;
    for(int i = from; i <= to; ++i)
    {
        result *= lambda(i);
    }
    return result;
}

template<>
inline rational prod(int from, int to, const std::vector<rational>& vector)
{
    rational result = 1_mpq;
    for(int i = from; i <= to; ++i)
        result *= vector[i];
    return result;
}



void compute_expected_for_all(prob_function p, std::string pd, prob_function q, std::string qd, int N)
{
    std::vector<rational> kappa(N, 1_mpq);
    std::vector<rational> pi(N, 1_mpq);
    std::vector<rational> dee(N, 1_mpq);
    std::vector<rational> zee(N, 1_mpq);
    
    for(int r = 1; r < N; ++r)
        kappa[r] = kappa[r-1] * q(r, N);
    for(int r = 1; r < N; ++r)
        pi[r] = pi[r-1] * p(r, N);
    for(int r = 0; r < N; ++r)
        dee[r] = kappa[r] / pi[r];
    for(int r = 1; r < N; ++r)
        zee[r] = pi[r-1] / kappa[r];
    
    // probabilities for all starting points:
    std::vector<rational> sums(N+1, 0_mpq);
    for(int n = 1; n <= N; ++n)
        sums[n] = sums[n-1] + dee[n-1];
    
    std::vector<rational> prob(N, 0_mpq);
    for(int i = 1; i < N; ++i)
        prob[i] = sums[i] / sums[N];
    
    // Running time (EX, EX^2)
    
    // EX (tau)
    std::vector<rational> taus(N, 0_mpq);
    // taus[0] shall remain 0
    taus[1] = sum(1, N-1, [&](int k) -> rational { return dee[k] * sum(1, k, [&](int i) -> rational { return 1_mpq / (p(i, N) * dee[i]); }); }) /
            sum(0, N-1, dee);
    
    for(int m = 1; m < N-1; ++m)
        taus[m+1] = taus[m] + q(m, N) / p(m, N) * (taus[m] - taus[m-1] - (1_mpq / q(m, N)));
    
    // EX^2 (v)
    std::vector<rational> zs(N, 0_mpq);
    zs[0] = 1_mpq;
    for(int k=1; k<N; ++k)
        zs[k] = 2*(taus[k] - 1) / q(k, N);
    
    std::vector<rational> vs(N, 0_mpq);
    // vs[0] shall remain 0 -- tau[0] is constantly equal to 0.
    vs[1] = sum(1, N-1,
                [&](int k) -> rational { return
                        dee[k] * sum(1, k, [&](int i) -> rational { return 1_mpq / (p(i, N) * dee[i]); }) +
                        zs[k] / dee[k-1] * sum(k, N-1, dee); }) /
              sum(0, N-1, dee);
    for(int k = 1; k < N-1; ++k)
        vs[k+1] = vs[k] + q(k,N)/p(k,N) * (vs[k] - vs[k-1] - 1_mpq/q(k,N) - zs[k]);
    
    std::vector<rational> vars(N, 0_mpq);
    for(int k = 1; k < N; ++k)
        vars[k] = vs[k] - taus[k] * taus[k];
    
    // EX (conditioned on win, vee-s)
    std::vector<rational> zee_sum(N, 0_mpq);
    for(int k = 1; k < N; ++k)
        zee_sum[k] = zee_sum[k-1] + zee[k];
    
    std::vector<rational> vee(N + 1, 0_mpq);
    for(int k = N-1; k > 0; --k)
        vee[k] = vee[k+1] + dee[k] * (zee_sum[k] - 1_mpq);
    
    
    auto saved = std::cerr.precision();
    std::cerr.precision(dbl::max_digits10 + 2);
    for(int i = 1; i < N; ++i)
    {
        std::cerr << pd << ";" << qd << ";" << i << ";" << N << ";" << prob[i].get_d() << ";" << taus[i].get_d() << ";" << vars[i].get_d() << ";" << vee[i].get_d() << "\n";
    }
    std::cerr.precision(saved);
}