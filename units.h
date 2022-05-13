#ifndef UNITS_H_
#define UNITS_H_

#include <type_traits>

template <int ... Values>
struct static_vector { 
    using type = static_vector; 
};

using empty_vector = static_vector<>;



/* push_back */

template <int Added, typename Vector>
struct push_back;

template <int Added, int ... Values>
struct push_back
<
    Added,
    static_vector<Values ...>
>
    : static_vector<Values ..., Added>
{};



/* prepend (push to the front) */

template <int Added, typename Vector>
struct prepend;

template <int Added, int ... Values>
struct prepend
<
    Added,
    static_vector<Values ...>
>
    : static_vector<Added, Values ...>
{};



/*
* Given a size (N), and an index to
* initialize the vector representing
* unit, initialize a static_vector
* instance with 1 in PlaceToAdd
* index and 0 in all other places.
*/

template <int N, int PlaceToAdd>
struct init 
    : push_back
        <
            N == PlaceToAdd,
            typename init
            <
                N - 1,
                PlaceToAdd
            >::type
        >::type
{};

template<int PlaceToAdd>
struct init<0, PlaceToAdd>
    : static_vector<PlaceToAdd == 0>
{};


/*
* check if two vectors have the same length
*/

template <typename Vector1, typename Vector2>
struct is_same_vec_len
{};


template<
    int Value1,
    int ... Value1s,
    int Value2,
    int ...Value2s
>
struct is_same_vec_len
<
    static_vector<Value1, Value1s...>,
    static_vector<Value2, Value2s...>
>
{
    static constexpr bool value = true &&
        is_same_vec_len
            <
                static_vector<Value1s...>,
                static_vector<Value2s...>
            >::value;
};

template<>
struct is_same_vec_len<empty_vector, empty_vector>
{
    static constexpr bool value = true;
};

template<int ...Value2s>
struct is_same_vec_len
<
    empty_vector,
    static_vector<Value2s...>
>
{
    static constexpr bool value = false;
};


template<int ...Value1s>
struct is_same_vec_len
<
    static_vector<Value1s...>,
    empty_vector
>
{
    static constexpr bool value = false;
};



/*
*  Given two static vectors x and y, add x_i
*  and y_i for all i in len(x)
*/

template<typename Vector1, typename Vector2>
    requires requires (Vector1, Vector2) { is_same_vec_len<Vector1, Vector2>::value; }
struct add
{};

template<
    int Value1,
    int ... Value1s,
    int Value2,
    int ... Value2s
>
struct add
<
    static_vector<Value1, Value1s ...>,
    static_vector<Value2, Value2s ...>
>
    : prepend
        <
            Value1 + Value2, 
            typename add
            <
                static_vector<Value1s ...>,
                static_vector<Value2s ...>
            >::type 
        >::type
{};

template<>
struct add<empty_vector, empty_vector>
    : empty_vector
{};



/* 
* Given a variadic numbers of Units and an
* initial accumulator static vector, add in
* all the exponents of all the units given.
* used by multiplied_unit
*/

template<typename Vector, typename ... Units>
struct accumulate;

template<
    int ... Values,
    typename TUnit1,
    typename ... TOtherUnits
>
struct accumulate
<
    static_vector<Values ...>,
    TUnit1, TOtherUnits ...
> 
    : add
        <
            static_vector<Values ...>,
            typename accumulate
            <
                typename TUnit1::exponents::type,
                TOtherUnits ...
            >::type
        >::type
{};

template<
    int ... Values,
    typename ... TOtherUnits
>
struct accumulate
<
    static_vector<Values ...>,
    TOtherUnits ...
>
    : static_vector<Values ...>
{};



/*
*  Given two static vectors x and y, subtract y_i
*  from x_i for all i in len(x)
*/

template<typename Vector1, typename Vector2>
    requires requires(Vector1, Vector2) { is_same_vec_len<Vector1, Vector2>::value; }
struct subtract
{};

template <
    int Value1,
    int ... Value1s,
    int Value2,
    int ... Value2s
>
struct subtract
<
    static_vector<Value1, Value1s ...>,
    static_vector<Value2, Value2s ...>
>
    : prepend
        <
            Value1 - Value2, 
            typename subtract
            <
                static_vector<Value1s ...>,
                static_vector<Value2s ...>
            >::type 
        >::type
{};

template<>
struct subtract<empty_vector, empty_vector>
    : empty_vector
{};



/* 
* is_same_vec
* check if two static_vectors have the same
* elements inside it.
*/

template<typename Vector1, typename Vector2>
struct is_same_vec;

template<
    int Value1,
    int ... Value1s,
    int Value2,
    int ... Value2s
>
struct is_same_vec
<
    static_vector<Value1, Value1s ...>,
    static_vector<Value2, Value2s ...>
>
{
    static constexpr bool value =
        (Value1 == Value2) &&
        is_same_vec
        <
            static_vector<Value1s ...>,
            static_vector<Value2s ...>
        >::value;
};

template<int Value1, int Value2>
struct is_same_vec
<
    static_vector<Value1>,
    static_vector<Value2>
>
{
    static constexpr bool value = Value1 == Value2;
};



/* units */

template<typename TEnum, typename TPowers>
    requires requires (TEnum) { std::is_enum_v<TEnum>; TEnum::_count; }
struct unit {
    using tenum = TEnum;
    using exponents = typename TPowers::type;
};

template<
    typename TFirstUnit,
    typename ... TOtherUnits
>
using multiplied_unit = unit
<
    typename TFirstUnit::tenum,
    typename accumulate
    <
        typename TFirstUnit::exponents,
        TOtherUnits ...
    >::type
>;



template<
    typename TDividendUnit,
    typename TDivisorUnit
>
using divided_unit = unit
<
    typename TDividendUnit::tenum,
    typename subtract
    <
        typename TDividendUnit::exponents,
        typename TDivisorUnit::exponents
    >::type
>;



template<typename TEnum, TEnum Index>
    requires requires (TEnum) { std::is_enum_v<TEnum>; TEnum::_count; }
using basic_unit = unit
<
    TEnum, 
    init
    <
        static_cast<int>(TEnum::_count) - 1,
        static_cast<int>(Index)
    >
>;


/* Quantity struct that allows for
* storing quantity for a given unit
* (basic, divided, multiplied units).
* Supports arithmetic (+, -, *, /)
* but only for quantities represented
* by the same enum class, i.e. TUnit
* must be the same.
* +, - : only for quantities with the
*        same unit. eg: metre + metre
*        is allowed but metre + second
*        is not.
*
* *, / : quantities with different units
*        are allowed too. using these
*        operators will change the
*        exponents. i.e: 
*        metre * metre = (metre) ^ 2 
*/

template<
    typename TUnit,
    typename TValue = double
>
struct quantity {

    TValue _value;
    using unit = TUnit;
    using tvalue = TValue;

    explicit quantity(TValue value)
        : _value(value) {}

    TValue value() {
        return _value;
    }
};



template<typename TQuantity1, typename TQuantity2>
concept SameUnit = is_same_vec
<
    typename TQuantity1::unit::exponents,
    typename TQuantity2::unit::exponents
>::value;

template<typename TQuantity1, typename TQuantity2>
concept SameEnum = std::is_same
<
    typename TQuantity1::unit::tenum,
    typename TQuantity2::unit::tenum
>::value;




template<
    typename TQuantity1,
    typename TQuantity2
>
requires SameUnit<TQuantity1, TQuantity2>
    && SameEnum<TQuantity1, TQuantity2>

inline auto operator +(TQuantity1 q1, TQuantity2 q2) {
    return quantity
    <
        typename TQuantity1::unit,
        typename TQuantity1::tvalue
    >
    (q1.value() + q2.value());
}



template<
    typename TQuantity1,
    typename TQuantity2
>
requires SameUnit<TQuantity1, TQuantity2>
    && SameEnum<TQuantity1, TQuantity2>

inline auto operator -(TQuantity1 q1, TQuantity2 q2) {
    return quantity
    <
        typename TQuantity1::unit,
        typename TQuantity1::tvalue
    >
    (q1.value() - q2.value());
}



template<
    typename TQuantity1,
    typename TQuantity2
>
requires SameEnum<TQuantity1, TQuantity2>

inline auto operator *(TQuantity1 q1, TQuantity2 q2) {
    return quantity
    <
        multiplied_unit
        <
            typename TQuantity1::unit,
            typename TQuantity2::unit
        >,
        typename TQuantity1::tvalue
    >
    (q1.value() * q2.value());
}



template<
    typename TQuantity1,
    typename TQuantity2
>
requires SameEnum<TQuantity1, TQuantity2>

inline auto operator /(TQuantity1 q1, TQuantity2 q2) {
    return quantity
    <
        divided_unit
        <
            typename TQuantity1::unit,
            typename TQuantity2::unit
        >,
        typename TQuantity1::tvalue
    >
    (q1.value() / q2.value());
}


#endif