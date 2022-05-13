## Type system for physical units

The header file contains a set of templates, that allows a user to make their own set of units, e.g. SI.
Each variable will have one of the following types of units:

1. Scalar, where the variable explicitly has no unit
2. Base unit (e.g.  mass, kilogram, seconds)
3. Derived unit (e.g. Force, Acceleration, etc)

### Representation

The units are internally represented in terms of integer exponents using the 'static_vector' struct.

For example, in SI unit, we have 7 basic units
```c++
[s, m, kg, A, K, mol, cd]
```

__A scalar will internally be represented as__
```c++
static_vector<0, 0, 0, 0, 0, 0, 0>
```

__Time, in seconds will be represented as__
```c++
static_vector<1, 0, 0, 0, 0, 0, 0>
```

__Force, in Newton kg . m . s <sup>-2</sup>, will be represented as__
```c++
static_vector<-2, 1, 1, 0, 0, 0, 0>
```

## Sample Usage

```c++
#include "units.h"
#include <iostream>

enum class si_units
{
    second,
    metre,
    kilogram,
    ampere,
    kelvin,
    mole,
    candela,
    _count
};


using second = basic_unit<si_units, si_units::second>;
using metre = basic_unit<si_units, si_units::metre>;
using kilogram = basic_unit<si_units, si_units::kilogram>;

using metre_per_second = divided_unit<metre, second>;
using newton = divided_unit<multiplied_unit<kilogram, metre>, multiplied_unit<second, second>>;


int main()
{
    quantity<metre> l(2.1);
    quantity<second> t(0.9);
    auto v1(l / t);

    quantity<metre_per_second> v2{ 2.5 };

    std::cout << (v1 + v2).value() << std::endl;

    // Won't compile
    //std::cout << (l + t).value() << std::endl;
}
```