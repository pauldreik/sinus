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

enum class DataKind {
    /// all values are 1e-30 or smaller
    TINY,
    ///linearly increasing 0 - 100*pi, realistic for my application
    INCREASING,
    /// binary garbage
    GARBAGE
};

// makes Nelements of type T with data
template<typename T>
std::vector<T> makeData(std::size_t Nelements,DataKind kind, int seed) {
    // worst seeding code ever :-(
    std::seed_seq seeder{seed};
    std::mt19937_64 engine(seeder);
    std::vector<T> ret;
    ret.reserve(Nelements);
    switch(kind) {
    case DataKind::TINY:
    {
        std::uniform_real_distribution<T> uniform_dist(-1e-30,1e-30);
        for(size_t i=0; i<Nelements ; ++i) {
            ret.emplace_back(uniform_dist(engine));
        }
    }
        break;
    case DataKind::INCREASING:
    {
        const auto endvalue=100*M_PI;
        const double step=endvalue/Nelements;
        double x=0;
        for(size_t i=0; i<Nelements ; ++i) {
            ret.emplace_back(x);
            x+=step;
        }
    }
        break;
    case DataKind::GARBAGE:
    {
        for(size_t i=0; i<Nelements ; ++i) {
            using Output=decltype (engine());
            constexpr auto Nbytes=std::lcm(sizeof(T),sizeof (Output));
            constexpr auto Noutput=Nbytes/sizeof(Output);
            constexpr auto N_of_T=Nbytes/sizeof(T);
            Output tmpbin[Noutput];
            for(auto& e: tmpbin) {
                e=engine();
            }

            T val[N_of_T];
            std::memcpy(&val,&tmpbin,Nbytes);
            for(size_t j=0; j<N_of_T && i<Nelements;++j,++i) {
                ret.emplace_back(val[j]);
            }
        }
    }
        break;
    default:
        throw std::runtime_error("unimplemented kind");
    }

    return ret;
}


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

    for(auto e : {DataKind::TINY,DataKind::INCREASING,DataKind::GARBAGE}) {
        runBenchmark<float,VectorLibrary>(3000,e,seed);
    }
}
