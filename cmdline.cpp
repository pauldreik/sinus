#include <iostream>
#include <array>
#include <iomanip>

#include "VectorLibrary.hpp"

int main(int argc, char*argv[]) {

    //read doubles on text format on stdin,
    //let sin() operate on them and output on stdout

    std::cout<<std::scientific<<std::setprecision(18);

    size_t total_count=0;
    while(std::cin) {
        std::array<double,20> buffer;

        std::size_t count=0;
        double tmp;
        while(count<buffer.size() && std::cin>>tmp) {
            buffer[count++]=tmp;
        }

        PaulVectorLibrary::inplace_sin(buffer.data(),count);

        for(std::size_t i=0; i<count ; ++i) {
            std::cout<<buffer[i]<<'\n';
        }
        total_count+=count;
    }
    std::cerr<<"wrote "<<total_count<<" values\n";
}
