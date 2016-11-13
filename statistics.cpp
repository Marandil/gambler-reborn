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

void compute_expected_for_all(prob_function p, std::string pd, prob_function q, std::string qd, int N)
{
    // TODO: probabilities for all starting points.
    std::vector<rational> prod(N+1, 1_mpq);
    for(int r = 1; r < N; ++r)
        prod[r] = prod[r-1] * q(r, N) / p(r, N);
    
    std::vector<rational> sums(N+1, 0_mpq);
    for(int n = 1; n <= N; ++n)
        sums[n] = sums[n-1] + prod[n-1];
    
    std::vector<rational> prob(N, 0_mpq);
    for(int i = 1; i < N; ++i)
        prob[i] = sums[i] / sums[N];
    
    // Running time (EX, EX^2)
    
    // EX (tau)
    std::vector<rational> ds(N, 0_mpq);
    ds[0] = 1_mpq;
    for(int i=1; i<N; ++i)
        ds[i] = ds[i-1]*q(i,N)/p(i,N);
    
    std::vector<rational> taus(N, 0_mpq);
    // taus[0] shall remain 0
    taus[1] = sum(1, N-1, [&](int k) -> rational { return ds[k] * sum(1, k, [&](int i) -> rational { return 1_mpq / (p(i, N) * ds[i]); }); }) /
            sum(0, N-1, ds);
    
    //for(int m = 2; m < N; ++m)
        //taus[m] = taus[1]*sum(0, m-1, [&](int k) { return ds[k]; }) -
        //        sum(1, m-1, [&](int k) -> rational { return ds[k] * sum(1, k, [&](int i) -> rational { return 1_mpq / (p(i, N) * ds[i]); }); });
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
                        ds[k] * sum(1, k, [&](int i) -> rational { return 1_mpq / (p(i, N) * ds[i]); }) +
                        zs[k] / ds[k-1] * sum(k, N-1, ds); }) /
              sum(0, N-1, ds);
    for(int k = 1; k < N-1; ++k)
        vs[k+1] = vs[k] + q(k,N)/p(k,N) * (vs[k] - vs[k-1] - 1_mpq/q(k,N) - zs[k]);
    
    std::vector<rational> vars(N, 0_mpq);
    for(int k = 1; k < N; ++k)
        vars[k] = vs[k] - taus[k] * taus[k];
    
    auto saved = std::cerr.precision();
    std::cerr.precision(dbl::max_digits10 + 2);
    for(int i = 1; i < N; ++i)
    {
        std::cerr << pd << ";" << qd << ";" << i << ";" << N << ";" << prob[i].get_d() << ";" << taus[i].get_d() << ";" << vars[i].get_d() << "\n";
    }
    std::cerr.precision(saved);
}