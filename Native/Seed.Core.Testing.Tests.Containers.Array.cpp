// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Containers.Array.h"

#pragma region NonConcurency Array

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::ForEachTest::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10)).All(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::ForEachFromTest::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::ForEachFromToTest::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 8, 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::AgregateSimpleItemsTypeTest::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.Aggregate(ArrowFunct(x, y, x += y)) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 0; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::AgregateConvertionsItemsTypeTest::Excecute() {
	auto arr = Core::Containers::Array<String>::Generate(ArrowFunct(i, String::toString(i)), 10);
	return arr.Aggregate<SizeT>(ArrowFunct(x, y, x += y.toInt<SizeT>())) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 0; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::AnyEmpty::Excecute() {
	return !Core::Containers::Blob().Any();
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::AnyWithItem::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Any();
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::AnyWithCondition::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 5));
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::AllWithCondition::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).All(ArrowFunct(x, x < 10));
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::CloneArrayWithPods::Excecute() {
	auto orig = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	auto copy = orig.Clone();

	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < orig.Count; i++)
		{
			if (orig[i] != copy[i])
				return false;
		}

		return true;
	});

	return true;
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::CloneArrayWithClonableItems::Excecute() {
	auto orig = Core::Containers::Array<String>::Generate(ArrowFunct(i, String::toString(i)), 10);
	auto copy = orig.Clone();

	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < orig.Count; i++)
		{
			if (orig[i] != copy[i])
				return false;
		}

		return true;
	});

	return true;
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::ResizeArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Resize(5).All(ArrowFunct(i, i < 5));
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::FirstCondition::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, true)) == 0;
}

bool Seed::Core::Testing::Test::Containers::Array::NonConcurency::LastCondition::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, true)) == 9;
}

#pragma endregion

#pragma region Concurency Array

bool Seed::Core::Testing::Test::Containers::Array::Concurency::ForEachTest::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10)).All(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::ForEachFromTest::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::ForEachFromToTest::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 8, 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::AgregateSimpleItemsTypeTest::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.Aggregate(ArrowFunct(x, y, x += y)) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 0; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::AgregateConvertionsItemsTypeTest::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<String>::Generate(ArrowFunct(i, String::toString(i)), 10);
	return arr.Aggregate<SizeT>(ArrowFunct(x, y, x += y.toInt<SizeT>())) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 0; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::AnyEmpty::Excecute() {
	return !Core::Containers::ConcurencyArray<Int32>().Any();
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::AnyWithItem::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Any();
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::AnyWithCondition::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 5));
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::AllWithCondition::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).All(ArrowFunct(x, x < 10));
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::CloneArrayWithPods::Excecute() {
	auto orig = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	auto copy = orig.Clone();

	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < orig.Count; i++)
		{
			if (orig[i] != copy[i])
				return false;
		}

		return true;
	});

	return true;
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::CloneArrayWithClonableItems::Excecute() {
	auto orig = Core::Containers::ConcurencyArray<String>::Generate(ArrowFunct(i, String::toString(i)), 10);
	auto copy = orig.Clone();

	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < orig.Count; i++)
		{
			if (orig[i] != copy[i])
				return false;
		}

		return true;
	});

	return true;
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::ResizeArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Resize(5).All(ArrowFunct(i, i < 5));
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::FirstCondition::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, true)) == 0;
}

bool Seed::Core::Testing::Test::Containers::Array::Concurency::LastCondition::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, true)) == 9;
}

#pragma endregion