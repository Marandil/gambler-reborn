#include <cstdlib>
#include <string>

#include <iostream>

struct ppf
{
    void* ptr_p;
    void* ptr_q;
    std::string p;
    std::string q;
    
    // ppf() : ptr_p(nullptr), ptr_q(nullptr), p(0), q(0) { }
    // ppf() = default;
    // ppf(void*, void*, int, int) = default;
    // ppf(ppf&&) = default;
    // ppf(const ppf&) = default;
    
    static ppf alloc(double pp, double qq)
    {
        //ppf r = {new double(pp), new double(qq), "pp", "qq"};
        //return r;
        return ppf{new double(pp), new double(qq), "pp", "qq"};
    }
};

int main()
{
    ppf test = ppf::alloc(1.7, 1.8);
    std::cout << test.ptr_p << "\n" << test.ptr_q << "\n" << test.p << "\n" << test.q << "\n";
    ppf test2;
    std::cout << test2.ptr_p << "\n" << test2.ptr_q << "\n" << test2.p << "\n" << test2.q << "\n";
}
