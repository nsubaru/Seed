// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

#include "Seed.Core.Testing.Tests.Containers.Array.h"

//TODO: rewrite without using deprecated test return state

#pragma region NonConcurency Array

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::ForEachTestArray::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	Asserts::AssertTrue(arr.ForEach(ArrowFunct(x, x * 10)).All(ArrowFunct(x, x < 10)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::ForEachFromTestArray::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::ForEachFromToTestArray::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 8, 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::AgregateSimpleItemsTypeTestArray::Excecute() {
	auto arr = Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.Aggregate(ArrowFunct(x, y, x + y)) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::AgregateConvertionsItemsTypeTestArray::Excecute() {
	auto arr = Core::Containers::Array<String>::Generate(ArrowFunct(i, String::toString(i)), 10);
	return arr.Aggregate(ArrowFunct(x, y, x + y.toInt<SizeT>()), 0Sz) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::AnyEmptyArray::Excecute() {
	return !Core::Containers::Blob().Any();
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::AnyWithItemArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Any();
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::AnyWithConditionArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 5));
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::AllWithConditionArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).All(ArrowFunct(x, x < 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::CloneArrayWithPods::Excecute() {
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

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::CloneArrayWithClonableItems::Excecute() {
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

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::ResizeArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Resize(5).All(ArrowFunct(i, i < 5));
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::FirstConditionArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, true)) == 0;
}

bool Seed::Core::Testing::Test::API::Containers::Array::NonConcurency::LastConditionArray::Excecute() {
	return Core::Containers::Array<SizeT>::Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, true)) == 9;
}

#pragma endregion

#pragma region Concurency Array

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::ForEachTestArray::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return !arr.ForEach(ArrowFunct(x, x * 10)).All(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::ForEachFromTestArray::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::ForEachFromToTestArray::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.ForEach(ArrowFunct(x, x * 10), 8, 9)
			  .Any(ArrowFunct(x, x > 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::AgregateSimpleItemsTypeTestArray::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10);
	return arr.Aggregate(ArrowFunct(x, y, x + y)) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::AgregateConvertionsItemsTypeTestArray::Excecute() {
	auto arr = Core::Containers::ConcurencyArray<String>::Generate(ArrowFunct(i, String::toString(i)), 10);
	return arr.Aggregate(ArrowFunct(x, y, x + y.toInt<SizeT>()), 0Sz) == [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}();
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::AnyEmptyArray::Excecute() {
	return !Core::Containers::ConcurencyArray<Int32>().Any();
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::AnyWithItemArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Any();
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::AnyWithConditionArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 5));
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::AllWithConditionArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).All(ArrowFunct(x, x < 10));
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::CloneArrayWithPods::Excecute() {
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

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::CloneArrayWithClonableItems::Excecute() {
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

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::ResizeArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10)
		.Resize(5)
		.All(ArrowFunct(i, i < 5));
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::FirstConditionArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, true)) == 0;
}

bool Seed::Core::Testing::Test::API::Containers::Array::Concurency::LastConditionArray::Excecute() {
	return Core::Containers::ConcurencyArray<SizeT>::Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, true)) == 9;
}

#pragma endregion