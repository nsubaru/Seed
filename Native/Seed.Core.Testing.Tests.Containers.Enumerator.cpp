// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Containers.Enumerator.h"
#include "Seed.Core.Containers.Concurency.FixedArray.h"
#include "Seed.Core.Containers.Concurency.Map.h"
#include "Seed.Core.Containers.Concurency.Dictionary.h"
#include "Seed.Core.Containers.Concurency.FixedStack.h"

bool Seed::Testing::Test::Containers::Enumerator::Excecute()
{
	auto ListForFlag = true;
	auto ArrayForFlag = true;

	auto list = Core::Containers::ConcurencyList<Int32>({ 1,2,3,4,5,6,7,8,9 });

    list.ForEach([&](const Int32& Value, SizeT Index) {
        if (ListForFlag && static_cast<SizeT>(Value) != Index + 1)
            ListForFlag = false;
        });

    auto array = Core::Containers::ConcurencyArray<Int32>({ 1,2,3,4,5,6,7,8,9 });

    array.ForEach([&](const Int32& Value, SizeT Index) {
        if (ArrayForFlag && static_cast<SizeT>(Value) != Index + 1)
            ArrayForFlag = false;
        });

	const auto AggregateCmp =
        list.Aggregate([](Int32& Value, const Int32& Item) {Value += Item; }, 0) == array.Aggregate([](Int32& Value, const Int32& Item) {Value += Item; }, 0);

	const auto AllLess =
        list.All([](const Int32& Value) {return Value < 10; }) && array.All([](const Int32& Value) {return Value < 10; });

	const auto AnyLess =
        list.Any([](const Int32& Value) {return Value < 10; }) && array.All([](const Int32& Value) {return Value < 10; });

    const auto  Any =
        list.Any() && array.Any();

    const auto  IsFirstValid =
        list.First([](const Int32& Value) {return Value < 10; }) == 1 &&
        array.First([](const Int32& Value) {return Value < 10; }) == 1;

    const auto  IsLastValid =
        list.Last([](const Int32& Value) {return Value < 10; }) == 9 &&
        array.Last([](const Int32& Value) {return Value < 10; }) == 9;

    list.SortBy(false);
    array.SortBy(false);

    const auto AggregateCmpSorted =
        list.Aggregate([](Int32& Value, const Int32& Item) {Value += Item; }, 0) == array.Aggregate([](Int32& Value, const Int32& Item) {Value += Item; }, 0);

    const auto AllLessSorted =
        list.All([](const Int32& Value) {return Value < 10; }) && array.All([](const Int32& Value) {return Value < 10; });

    const auto AnyLessSorted =
        list.Any([](const Int32& Value) {return Value < 10; }) && array.All([](const Int32& Value) {return Value < 10; });

    const auto AnySorted =
        list.Any() && array.Any();

    const auto IsFirstValidSorted =
        list.First([](const Int32& Value) {return Value < 10; }) == 9 &&
        array.First([](const Int32& Value) {return Value < 10; }) == 9;

    const auto IsLastValidSorted =
        list.Last([](const Int32& Value) {return Value < 10; }) == 1 &&
        array.Last([](const Int32& Value) {return Value < 10; }) == 1;

    array.Reverse(); 
    list.Reverse();

    const auto AggregateCmpReversed =
        list.Aggregate([](Int32& Value, const Int32& Item) {Value += Item; }, 0) == array.Aggregate([](Int32& Value, const Int32& Item) {Value += Item; }, 0);

    const auto AllLessReversed =
        list.All([](const Int32& Value) {return Value < 10; }) && array.All([](const Int32& Value) {return Value < 10; });

    const auto AnyLessReversed =
        list.Any([](const Int32& Value) {return Value < 10; }) && array.All([](const Int32& Value) {return Value < 10; });

    const auto AnyReversed =
        list.Any() && array.Any();

    const auto IsFirstValidReversed =
        list.First([](const Int32& Value) {return Value < 10; }) == 1 &&
        array.First([](const Int32& Value) {return Value < 10; }) == 1;

    const auto IsLastValidReversed =
        list.Last([](const Int32& Value) {return Value < 10; }) == 9 &&
        array.Last([](const Int32& Value) {return Value < 10; }) == 9;

    const auto IsComplexAggregateCorrect =
		    Core::Containers::ConcurencyArray<Int32>{ 1, 2, 3, 4, 5, 6, 7, 8, 9 }
            .Reverse()
            .ForEach([](int& v) { v += 10; })
            .Aggregate([](Int32& value, const Int32& item) { value += item - 10; }, 0)
    == 
        Core::Containers::ConcurencyList<Int32>{ 1, 2, 3, 4, 5, 6, 7, 8, 9 }
            .CopyTo<Core::Containers::ConcurencyArray>()
            .ForEach([](int& v) { v += 10; })
            .MoveTo<Core::Containers::ConcurencyList>()
            .Aggregate([](Int32& value, const Int32& item) { value += item - 10; }, 0);

    const auto IsFixedLinqWorkProperly =
		         Core::Containers::ConcurencyFixedArray<10, Int32>{1,2,3,4,5,6,7,8,9}
            .CopyToFixed<Core::Containers::ConcurencyFixedArray>()
            .Reverse()
            .MoveToFixed<Core::Containers::ConcurencyFixedArray>()
            .ForEach([](int& v) {v += 10; })
            .Aggregate([](Int32& value, const Int32& item) { value += item - 10; }, 0)
    ==
        Core::Containers::ConcurencyList<Int32>{ 1, 2, 3, 4, 5, 6, 7, 8, 9 }
            .CopyTo<Core::Containers::ConcurencyArray>()
            .ForEach([](int& v) { v += 10; })
            .MoveTo<Core::Containers::ConcurencyList>()
            .Aggregate([](Int32& value, const Int32& item) { value += item - 10; }, 0);

    auto map = Core::Containers::ConcurencyMap<String, Int32>{ {L"123",123}, {L"124", 124}, {L"125", 125} };
	
    auto IsForEachWorkProperly = true;
    map.ForEach([&](const Core::Containers::BasePair<Memory::Heap, String, Int32>& pair) {
        IsForEachWorkProperly = IsForEachWorkProperly ? pair.Key.toInt() == pair.Value : false;
        });
	
    auto IsForEachKeyWorkProperly = true;
    map.ForEachKey([&](const String& key) {
        IsForEachKeyWorkProperly = IsForEachKeyWorkProperly ? map.IsExist(key) : false;
        });
	
    auto IsForEachValueWorkProperly = true;
    map.ForEachValue([&](const Int32& value) {
        if (IsForEachValueWorkProperly) {
            bool IsFind = false;
            map.ForEach([&](const Core::Containers::BasePair<Memory::Heap, String, Int32>& pair) {
                if (!IsFind)
                    if (pair.Value == value)
                        IsFind = true;
                });
            IsForEachValueWorkProperly = IsFind;
        }
        });

    auto dict = Core::Containers::ConcurencyDictionary<Int32>{ {L"123",123}, {L"124", 124}, {L"125", 125} };
	
    auto IsForEachDictionaryWorkProperly = true;
    dict.ForEach([&](const Core::Containers::BasePair<Memory::Heap, String, Int32>& pair) {
        IsForEachDictionaryWorkProperly = IsForEachDictionaryWorkProperly ? pair.Key.toInt() == pair.Value : false;
        });

	auto IsForEachKeyDictionaryWorkProperly = true;
    dict.ForEachKey([&](const String& key) {
        IsForEachKeyDictionaryWorkProperly = IsForEachKeyDictionaryWorkProperly ? dict.IsExist(key) : false;
        });

	auto IsForEachValueDictionaryWorkProperly = true;
    dict.ForEachValue([&](const Int32& value) {
        if (IsForEachValueDictionaryWorkProperly) {
            bool IsFind = false;
            dict.ForEach([&](const Core::Containers::BasePair<Memory::Heap, String, Int32>& pair) {
                if (!IsFind)
                    if (pair.Value == value)
                        IsFind = true;
                });
            IsForEachValueDictionaryWorkProperly = IsFind;
        }
        });

    auto stack = Core::Containers::ConcurencyFixedStack<10, Int32>({ 1,2,3 });

	const auto IsStackContaineAllItems = stack.Aggregate([](Int32& Result, const Int32& Value) {Result += Value; }) == 1 + 2 + 3;

    auto s1 = stack.Pop();
    auto s2 = stack.Pop();
    auto s3 = stack.Pop();

	const auto IsStackCorrectInit = s1 == 1 && s2 == 2 && s3 == 3;

    stack.Push(s3);
    stack.Push(s2);
    stack.Push(s1);

    s1 = stack.Pop();
    s2 = stack.Pop();
    s3 = stack.Pop();

	const auto IsStackCorrectPushAndPop = s1 == 1 && s2 == 2 && s3 == 3;

    return ListForFlag && ArrayForFlag && AggregateCmp && AllLess && AnyLess && AnyReversed && Any && IsFirstValid && IsLastValid &&
        AggregateCmpSorted && AllLessSorted && AnyLessSorted && AnySorted && IsFirstValidSorted && IsLastValidSorted &&
        AggregateCmpReversed && AllLessReversed && AnyLessReversed && Any && IsFirstValidReversed && IsLastValidReversed && 
        IsComplexAggregateCorrect && IsFixedLinqWorkProperly &&
        IsForEachWorkProperly && IsForEachKeyWorkProperly && IsForEachValueWorkProperly &&
        IsForEachDictionaryWorkProperly && IsForEachKeyDictionaryWorkProperly && IsForEachValueDictionaryWorkProperly &&
        IsStackContaineAllItems && IsStackCorrectInit && IsStackCorrectPushAndPop;
};
