#include "Value.hpp"
#include "CFunction.hpp"
#include "Object.hpp"

#include <iostream>

using namespace aya;
using namespace std;

struct A : public CustomAllocatedMemory { int state[23]; };
struct B : public A { int moreState[42]; };

Mallocator AllocatorProxy<Mallocator>::allocator;
thread_local CustomAllocatedMemory::Allocator* CustomAllocatedMemory::lastAllocator = nullptr;

int main(int argc, char* argv[]) {
    CustomAllocatedMemory* a = new B();
    delete a;
    
    return 0;
}