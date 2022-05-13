#include "units.h"
#include <iostream>

template<int ... Values>
constexpr unsigned size(static_vector<Values ...>)
{
    return sizeof...(Values);
}

template <int ...A>
void print_list(static_vector<A ...>)
{
    (void(std::cout << A << " ") , ...);
    std::cout << std::endl;
}


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

enum class si_units_2
{
    asd,
    def,
    _count
};

using _asd = basic_unit<si_units_2, si_units_2::asd>;
using _def = basic_unit<si_units_2, si_units_2::def>;


using second = basic_unit<si_units, si_units::second>;
using metre = basic_unit<si_units, si_units::metre>;
using kilogram = basic_unit<si_units, si_units::kilogram>;
using cubic_metre = multiplied_unit<metre, metre, metre, second, second, kilogram>;

using metre_per_second = divided_unit<metre, second>;
using newton = divided_unit<multiplied_unit<kilogram, metre>, multiplied_unit<second, second>>;

using new_newton = divided_unit<divided_unit<multiplied_unit<kilogram, metre>, second >, second>;


int main()
{
    //std::cout << static_cast<int>(si_units::_count) << std::endl;
    //std::cout << std::is_same<si_units, si_units>::value << std::endl;
    quantity<metre> _metre(2.1);
    quantity<_asd> _asdd(22);
    quantity<_def> _deff(33);
    quantity<metre> k(3.9);
    quantity<metre_per_second> o(44);
    quantity<divided_unit<multiplied_unit<metre>, second>> aa(33);
    quantity<newton> _newton(33);
    quantity<divided_unit<divided_unit<multiplied_unit<kilogram, metre>, second >, second>> new_newton(33);
    //using a = add<static_vector<1,2,3,4>, static_vector<3,4,5,6>>;
    //using b = push_back<55, static_vector<>>;

    //print_list(typename metre_per_second::exponents());
    std::cout << is_same_vec<typename second::exponents, typename newton::exponents >::value << std::endl;


    auto kp(_newton / new_newton);
    auto kpp(_deff * _asdd);

    using new_scalar = typename decltype(kp)::unit;

    si_units a(si_units::ampere);
    si_units b(si_units::candela);

    //std::cout <<  decltype(a) ==  decltype(b) << std::endl;




    //auto ll(_newton + _metre);
    auto ll2(_newton * _metre);
    //auto ll3(_newton - _metre);
    auto ll4(_newton / _metre);


    std::cout << (_newton / new_newton).value() << std::endl;


    static_assert(size(typename cubic_metre::exponents()) == 7, "Error");
 }