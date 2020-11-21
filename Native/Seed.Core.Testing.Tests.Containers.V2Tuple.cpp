#include "Seed.Core.Testing.Tests.Containers.V2Tuple.h"

#include "Seed.Core.Containers.V2Tuple.h"

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::CreateV2Leafs::Excecute() {
    constexpr auto v1 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, int, float>(1, 1.0);

    Asserts::AssertEqual(v1.Count, 2);

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::ConstexprCopyV2Leafs::Excecute() {
    constexpr auto v1 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, int, float>(1, 1.0);
    constexpr auto v2 = v1;

    Asserts::AssertEqual(v1.Count, v2.Count);

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::AccessToValuesV2Leafs::Excecute() {
    constexpr auto v1 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, int, float>(1, 1.0);
    constexpr auto v2 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, int, float>(1, 1.0);

    Asserts::AssertEqual(v1.GetByIdx<0>(), v2.GetByIdx<0>());
    Asserts::AssertEqual(v1.GetByIdx<1>(), v2.GetByIdx<1>());

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::AcessToValueRefV2Leafs::Excecute() {
    int a1 = 1;
    float a2 = 1.0;

    auto v1 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, int&, float&>(a1, a2);
    auto v2 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, int&, float&>(a1, a2);

    Asserts::AssertEqual(v1.GetByIdx<0>(), v2.GetByIdx<0>());
    Asserts::AssertEqual(v1.GetByIdx<1>(), v2.GetByIdx<1>());

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::AcessToValueConstRefV2Leafs::Excecute() {
    int a1 = 1;
    float a2 = 1.0;

    auto v1 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, const int&, const float&>(a1, a2);
    auto v2 = Core::Containers::Details::V2Leafs<TypeTraits::Indexer<2>::Type, const int&, const float&>(a1, a2);

    Asserts::AssertEqual(v1.GetByIdx<0>(), v2.GetByIdx<0>());
    Asserts::AssertEqual(v1.GetByIdx<1>(), v2.GetByIdx<1>());

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::CreateV2Tuple::Excecute() {
    constexpr auto v1 = Core::Containers::V2Tuple(1, 1.0, 'c');

    Asserts::AssertEqual(v1.Count, 3);

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::ConstexprCopyV2Tuple::Excecute() {
    constexpr auto v1 = Core::Containers::V2Tuple(1, 1.0, 'c');
    constexpr auto v2 = v1;

    Asserts::AssertEqual(v1.Count, v2.Count);

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::AccessToValuesV2Tuple::Excecute() {
    constexpr auto v1 = Core::Containers::V2Tuple(1, 1.0, 'c');
    constexpr auto v2 = v1;

    Asserts::AssertEqual(v1.GetByIdx<0>(), v2.GetByIdx<0>());
    Asserts::AssertEqual(v1.GetByIdx<1>(), v2.GetByIdx<1>());
    Asserts::AssertEqual(v1.GetByIdx<2>(), v2.GetByIdx<2>());

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::AcessToValueRefV2Tuple::Excecute() {
    int a1 = 1;
    float a2 = 1.0;
    char a3 = '1';

    auto v1 = Core::Containers::V2Tuple<int&, float&, char&>(a1, a2, a3);

    Asserts::AssertEqual(v1.GetByIdx<0>(), a1);
    Asserts::AssertEqual(v1.GetByIdx<1>(), a2);
    Asserts::AssertEqual(v1.GetByIdx<2>(), a3);

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::AcessToValueConstRefV2Tuple::Excecute() {
    int a1 = 1;
    float a2 = 1.0;
    char a3 = '1';

    auto v1 = Core::Containers::V2Tuple<const int&, const float&, const char&>(a1, a2, a3);

    Asserts::AssertEqual(v1.GetByIdx<0>(), a1);
    Asserts::AssertEqual(v1.GetByIdx<1>(), a2);
    Asserts::AssertEqual(v1.GetByIdx<2>(), a3);

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::ConstexprCallWithV2Tuple::Excecute() {
    constexpr auto v1 = Core::Containers::V2Tuple(1, 1.0, 'a');
    constexpr auto v2 = v1.CallWithArgsFromTuple([] (int a, double b, char c) constexpr { return (char) (c + (char) a + (char) b); });

    Asserts::AssertEqual(v2, 'c');

    return true;
}

bool Seed::Core::Testing::Test::API::Containers::V2Tuple::ConstextprV2TupleExpand::Excecute() {
    auto v1 = Core::Containers::V2Tuple(1, 1.0, 'a');

    auto [a, b, c] = v1;

    Asserts::AssertEqual(a, v1.GetByIdx<0>());
    Asserts::AssertEqual(b, v1.GetByIdx<1>());
    Asserts::AssertEqual(c, v1.GetByIdx<2>());

    return true;
}
