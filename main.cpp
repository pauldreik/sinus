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
            const auto tmpbin=engine();
            using Output=decltype (tmpbin);
            static_assert(sizeof(Output)==sizeof(T),"fix implementation");
            T val;
            std::memcpy(&val,&tmpbin,sizeof(tmpbin));
            ret.emplace_back(val);
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
    return speed;
}

// using std::sin
struct Std_sin : ImplementationBase {
    // tiny - 20 cycles/element
    // increasing - 30 cycles/element
    // garbage - 100 cycles/element (because of how double is represented, this is mostly huge numbers)
    static double evaluate_sin(const double* p, int n) {
        double sum=0;
        for(int i=0; i<n;++i) {
            sum +=std::sin(p[i]);
        }
        return sum;
    }
};

int main(int /*argc*/, char* /*argv*/[]) {
    const auto seed=static_cast<int>(std::random_device{}());
    //runBenchmark<double,VectorLibrary>(3000,DataKind::TINY,seed);
    runBenchmark<double,VectorLibrary>(3000,DataKind::GARBAGE,seed);
    //runBenchmark<double,Std_sin>(3000,DataKind::TINY,seed);
    //runBenchmark<double,Std_sin>(3000,DataKind::INCREASING,seed);
    //runBenchmark<double,Std_sin>(3000,DataKind::GARBAGE,seed);
}
