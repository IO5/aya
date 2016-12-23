#include "Value.hpp"
#include "CFunction.hpp"
#include "Object.hpp"

#include <iostream>

using namespace aya;
using namespace std;

int main(int argc, char* argv[]) {
    Value a = 0;
    cout << boolalpha << a.is<int_t>() << '\n';
    return 0;
}