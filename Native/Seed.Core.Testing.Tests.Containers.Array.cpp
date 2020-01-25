// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Containers.Array.h"

bool Seed::Core::Testing::Test::Containers::SimpleTypeArrayTest::Excecute()
{
	Seed::Containers::Array<Int32> array({ 1,2,3,4,5,6,7,8,9,10 });

	Int32 Sum = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;

	
	bool IsForEachWork = [&]() {Int32 tmp = 0; array.ForEach([&](const Int32& item) {tmp += item; }); return tmp; }() == Sum;
	bool IsAggregateWork = array.Aggregate([](Int32& result, const Int32& item) { result += item; }) == Sum;
	bool IsAnyWork = !array.Any([&](const Int32& item) { return item > 11; }) && array.Any([&](const Int32& item) {return item == 10; });
	bool IsAllWork = !array.All([&](const Int32& item) {return item > 11; }) && array.All([&](const Int32& item) {return item < 11; });
	bool IsFirstWork = array.First([](const Int32& item) {return item > 0; }) == 1;
	bool IsLastWork = array.Last([](const Int32& item) {return item > 0; }) == 10;
	bool IsResizeWork = [&]() mutable {
		SizeT Sum = 0;
		array.Resize(15);
		array.ForEach([&](Int32& item, SizeT Index) {item = (Int32)Index; Sum += Index; });
		return array.Aggregate([](Int32& result, const Int32& item) { result += item; }) == Sum;
	}();

	return IsForEachWork && IsAggregateWork && IsAnyWork && IsAllWork && IsFirstWork && IsLastWork && IsResizeWork;
}

bool Seed::Core::Testing::Test::Containers::SimpleTypeConcurencyArrayTest::Excecute()
{
	Seed::Containers::ConcurencyArray<Int32> array({ 1,2,3,4,5,6,7,8,9,10 });

	Int32 Sum = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;


	bool IsForEachWork = [&]() {Int32 tmp = 0; array.ForEach([&](const Int32& item) {tmp += item; }); return tmp; }() == Sum;
	bool IsAggregateWork = array.Aggregate([](Int32& result, const Int32& item) { result += item; }) == Sum;
	bool IsAnyWork = !array.Any([&](const Int32& item) { return item > 11; }) && array.Any([&](const Int32& item) {return item == 10; });
	bool IsAllWork = !array.All([&](const Int32& item) {return item > 11; }) && array.All([&](const Int32& item) {return item < 11; });
	bool IsFirstWork = array.First([](const Int32& item) {return item > 0; }) == 1;
	bool IsLastWork = array.Last([](const Int32& item) {return item > 0; }) == 10;
	bool IsResizeWork = [&]() mutable {
		SizeT Sum = 0;
		array.Resize(15);
		array.ForEach([&](Int32& item, SizeT Index) {item = (Int32)Index; Sum += Index; });
		return array.Aggregate([](Int32& result, const Int32& item) { result += item; }) == Sum;
	}();

	return IsForEachWork && IsAggregateWork && IsAnyWork && IsAllWork && IsFirstWork && IsLastWork && IsResizeWork;
}

bool Seed::Core::Testing::Test::Containers::ComplexTypeArrayTest::Excecute()
{
	Seed::Containers::Array<String> array({
	String::toString(1),
	String::toString(2),
	String::toString(3),
	String::toString(4),
	String::toString(5),
	String::toString(6),
	String::toString(7),
	String::toString(8),
	String::toString(9),
	String::toString(10)
		});

	Int32 Sum = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;


	bool IsForEachWork = [&]() {Int32 tmp = 0; array.ForEach([&](const String& item) {tmp += item.toInt(); }); return tmp; }() == Sum;
	bool IsAggregateWork = array.Aggregate([](Int32& result, const String& item) { result += item.toInt(); }, 0) == Sum;
	bool IsAnyWork = !array.Any([&](const String& item) { return item.Lenght > 2; }) && array.Any([&](const String& item) {return item.Lenght > 1; });
	bool IsAllWork = !array.All([&](const String& item) {return item.Lenght > 2; }) && array.All([&](const String& item) {return !item.IsEmpty; });
	bool IsFirstWork = array.First([](const String& item) {return !item.IsEmpty; }) == L"1";
	bool IsLastWork = array.Last([](const String& item) {return !item.IsEmpty; }) == L"10";
	bool IsResizeWork = [&]() mutable {
		SizeT Sum = 0;
		array.Resize(15);
		array.ForEach([&](String& item, SizeT Index) {item = String::toString(Index); Sum += Index; });
		return array.Aggregate([](Int32& result, const String& item) { result += item.toInt(); }, 0) == Sum;
	}();

	return IsForEachWork && IsAggregateWork && IsAnyWork && IsAllWork && IsFirstWork && IsLastWork && IsResizeWork;
}

bool Seed::Core::Testing::Test::Containers::ComplexTypeConcurencyArrayTest::Excecute()
{
	Seed::Containers::ConcurencyArray<String> array({
	String::toString(1),
	String::toString(2),
	String::toString(3),
	String::toString(4),
	String::toString(5),
	String::toString(6),
	String::toString(7),
	String::toString(8),
	String::toString(9),
	String::toString(10)
		});

	Int32 Sum = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;


	bool IsForEachWork = [&]() {Int32 tmp = 0; array.ForEach([&](const String& item) {tmp += item.toInt(); }); return tmp; }() == Sum;
	bool IsAggregateWork = array.Aggregate([](Int32& result, const String& item) { result += item.toInt(); }, 0) == Sum;
	bool IsAnyWork = !array.Any([&](const String& item) { return item.Lenght > 2; }) && array.Any([&](const String& item) {return item.Lenght > 1; });
	bool IsAllWork = !array.All([&](const String& item) {return item.Lenght > 2; }) && array.All([&](const String& item) {return !item.IsEmpty; });
	bool IsFirstWork = array.First([](const String& item) {return !item.IsEmpty; }) == L"1";
	bool IsLastWork = array.Last([](const String& item) {return !item.IsEmpty; }) == L"10";
	bool IsResizeWork = [&]() mutable {
		SizeT Sum = 0;
		array.Resize(15);
		array.ForEach([&](String& item, SizeT Index) {item = String::toString(Index); Sum += Index; });
		return array.Aggregate([](Int32& result, const String& item) { result += item.toInt(); }, 0) == Sum;
	}();

	return IsForEachWork && IsAggregateWork && IsAnyWork && IsAllWork && IsFirstWork && IsLastWork && IsResizeWork;
}
