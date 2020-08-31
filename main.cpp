#include <iostream>
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>
#include <random>
#include <cassert>
#include <cstring>


#include "ImplementationBase.hpp"
#include "VectorLibrary.hpp"

#include "GenerateData.hpp"

// returns the speed in elements/s
template<typename T, typename Implementation>
double runBenchmark(std::size_t Nelements,DataKind kind, int seed) {

    static_assert(std::is_base_of_v<ImplementationBase,Implementation>);



    const std::vector<T> data=makeData<T>(Nelements,kind,seed);
    const auto t0=std::chrono::system_clock::now();
    double sum=0;
    unsigned long long Nvals=0;

    // try running for approx. one second
    const auto nof_iterations=static_cast<int>(1.0*3e8/Nelements*8/sizeof(T));
    for(int i=0; i<nof_iterations; ++i) {
        int nelems=data.size()-(i&0b1111);
        assert(nelems>0 && nelems<=static_cast<int>(data.size()));
        sum+=Implementation::evaluate_sin(data.data(),nelems);
        Nvals+=nelems;
    }
    const auto t1=std::chrono::system_clock::now();
    const auto dt=std::chrono::duration_cast<std::chrono::duration<double>>(t1-t0).count();
    const auto speed=Nvals/dt;
    std::cout<<"the sum is "<<sum<<" speed is "<<Nvals<<" in "<<dt<<"="<<speed<<" values per second.\n";
    std::cout<<3e9/speed<<'\n';
    return speed;
}

// using std::sin
struct Std_sin : ImplementationBase {
    // tiny - 20 cycles/element
    // increasing - 30 cycles/element
    // garbage - 100 cycles/element (because of how double is represented, this is mostly huge numbers)
    static double evaluate_sin(const double* p, int n) {
        // loop unrolling does not really help performance
        double sum1=0;
        double sum2=0;
        double sum3=0;
        double sum4=0;
        int nfour=(n/4)*4;
        int i=0;
        for(; i<nfour;i+=4) {
            sum1 +=std::sin(p[i+0]);
            sum2 +=std::sin(p[i+1]);
            sum3 +=std::sin(p[i+2]);
            sum4 +=std::sin(p[i+3]);
        }
        for(; i<n;++i) {
            sum1 +=std::sin(p[i]);
        }
        return sum1+sum2+sum3+sum4;
    }
};


int main(int /*argc*/, char* /*argv*/[]) {


    const auto seed=static_cast<int>(std::random_device{}());
    //runBenchmark<double,VectorLibrary>(3000,DataKind::TINY,seed);
    //runBenchmark<double,VectorLibrary>(3000,DataKind::GARBAGE,seed);
//    runBenchmark<double,Std_sin>(3000,DataKind::TINY,seed);
//    runBenchmark<double,Std_sin>(3000,DataKind::INCREASING,seed);
//    runBenchmark<double,Std_sin>(3000,DataKind::GARBAGE,seed);

     runBenchmark<float,VectorLibrary>(3000,DataKind::MEDIUM,seed);
   return 0;
    for(auto e : {DataKind::TINY,DataKind::INCREASING,DataKind::GARBAGE}) {
        runBenchmark<float,VectorLibrary>(3000,e,seed);
    }
}
