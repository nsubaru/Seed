// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.h"

#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Testing.Tests.Containers.List.h"

#pragma region NonConcurency List

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::ForEachTestList::Excecute() {
	auto list = Core::Containers::List<SizeT>::Generate(ArrowFunct(idx, idx), 10);
	auto sum = 0Sz;
	list.ForEach(ArrowFunctWithClosure(x, sum += x));
	Asserts::AssertEqual(sum, 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	return true;
}


bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::RemoveIfTestList::Excecute() {
	auto list = Core::Containers::List<SizeT>::Generate(ArrowFunct(idx, idx), 10);
	list.RemoveIf(ArrowFunct(x, x > 5));
	SizeT sum = 0;
	for (auto i : list) sum += i;
	Asserts::AssertEqual(sum, 0 + 1 + 2 + 3 + 4 + 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::AgregateSimpleItemsTypeTestList::Excecute() {
	auto list = Core::Containers::List<SizeT>::Generate(ArrowFunct(idx, idx), 10);
	auto res = list.Aggregate(ArrowFunct(x, y, x + y));
	auto sum = 0Sz;
	for (auto i : list) sum += i;
	Asserts::AssertEqual(res, sum);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::AgregateConvertionsItemsTypeTestList::Excecute() {
	auto list = Core::Containers::List<String>::Generate(ArrowFunct(idx, String::toString(idx)), 10);
	auto res = list.Aggregate<SizeT>(ArrowFunct(x, y, x + y.toInt<SizeT>()));
	auto sum = 0Sz;
	for (auto i : list) sum += i.toInt<SizeT>();
	Asserts::AssertEqual(res, sum);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::AnyEmptyList::Excecute() {
	Asserts::AssertFalse(Core::Containers::List<SizeT>().Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::AnyWithItemList::Excecute() {
	Asserts::AssertTrue(Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10).Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::AnyWithConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 5)));
	Asserts::AssertFalse(Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 10)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::AllWithConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::List<SizeT>()
		.Generate(ArrowFunct(i, i), 10)
		.All(ArrowFunct(x, x >= 0)));
	Asserts::AssertFalse(Core::Containers::List<SizeT>()
		.Generate(ArrowFunct(i, i), 10)
		.All(ArrowFunct(x, x < 8)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::CloneListWithPods::Excecute() {
	auto list = Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10);
	auto clone = list.Clone();
	Asserts::AssertEqual(list.Count, clone.Count);
	Asserts::AssertTrue([&] () InlineLambda
	{
		auto listIt = list.FirstIt;
		auto cloneIt = list.FirstIt;
		while (listIt != nullptr && cloneIt != nullptr)
		{
			if ((*listIt) != (*cloneIt))
				return false;

			++listIt; ++cloneIt;
		}
		return !(list.IsSameAs(clone));
	});
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::CloneListWithClonableItems::Excecute() {
	auto list = Core::Containers::List<Core::Containers::List<String>>::Generate(
				ArrowFunct(
					Core::Containers::List<String>::Generate(
							ArrowFunct(x, String::toString(x)), 
					10)
				), 
				10);

	auto clone = list.Clone();
	Asserts::AssertEqual(list.Count, clone.Count);
	Asserts::AssertEqual(list.Aggregate<SizeT>(ArrowFunct(x, y, x + y.Count)), clone.Aggregate<SizeT>(ArrowFunct(x, y, x + y.Count)));
	Asserts::AssertEqual(
		list.Aggregate<String>(ArrowFunct(x, y, x + y.Aggregate(ArrowFunct(x, y, x + y)))),
		clone.Aggregate<String>(ArrowFunct(x, y, x + y.Aggregate(ArrowFunct(x, y, x + y)))));

	return !(list.IsSameAs(clone));
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::FirstConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, x >= 5)) == 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::LastConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, x <= 5)) == 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::ListIteratorsLoop::Excecute() {
	auto list = Core::Containers::List<SizeT>().Generate(ArrowFunct(i, i), 10);
	
	Asserts::AssertTrue([&] () InlineLambda
	{
		auto i = 0;
		for (auto& j : list)
		{
			if (i != j)
				return false;
			i++;
		}

		return true;
	});

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::MaxItem::Excecute() {
	return Core::Containers::List<SizeT>::Generate(ArrowFunct(i, i), 10).Max() == 9;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::MaxItemField::Excecute() {
	return Core::Containers::List<String>({ L"1"S, L"12"S, L"123"S }).Max(ArrowFunct(x, x.Lenght)) == 3;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::MaxByItemField::Excecute() {
	return Core::Containers::List<String>({ L"1"S, L"12"S, L"123"S }).MaxBy(ArrowFunct(x, x.Lenght)) == L"123";
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::MinItem::Excecute() {
	return Core::Containers::List<SizeT>::Generate(ArrowFunct(i, i), 10).Min() == 0;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::MinItemField::Excecute() {
	return Core::Containers::List<String>({ L"1"S, L"12"S, L"123"S }).Min(ArrowFunct(x, x.Lenght)) == 1;
}

bool Seed::Core::Testing::Test::API::Containers::List::NonConcurency::MinByItemField::Excecute() {
	return Core::Containers::List<String>({ L"1"S, L"12"S, L"123"S }).MinBy(ArrowFunct(x, x.Lenght)) == L"1";
}

#pragma endregion

#pragma region Concurency ConcurencyList

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::ForEachTestList::Excecute() {
	auto list = Core::Containers::ConcurencyList<SizeT>::Generate(ArrowFunct(idx, idx), 10);
	auto sum = 0Sz;
	list.ForEach(ArrowFunctWithClosure(x, sum += x));
	Asserts::AssertEqual(sum, 0 + 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	return true;
}


bool Seed::Core::Testing::Test::API::Containers::List::Concurency::RemoveIfTestList::Excecute() {
	auto list = Core::Containers::ConcurencyList<SizeT>::Generate(ArrowFunct(idx, idx), 10);
	list.RemoveIf(ArrowFunct(x, x > 5));
	SizeT sum = 0;
	for (auto i : list) sum += i;
	Asserts::AssertEqual(sum, 0 + 1 + 2 + 3 + 4 + 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::AgregateSimpleItemsTypeTestList::Excecute() {
	auto list = Core::Containers::ConcurencyList<SizeT>::Generate(ArrowFunct(idx, idx), 10);
	auto res = list.Aggregate(ArrowFunct(x, y, x + y));
	auto sum = 0Sz;
	for (auto i : list) sum += i;
	Asserts::AssertEqual(res, sum);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::AgregateConvertionsItemsTypeTestList::Excecute() {
	auto list = Core::Containers::ConcurencyList<String>::Generate(ArrowFunct(idx, String::toString(idx)), 10);
	auto res = list.Aggregate<SizeT>(ArrowFunct(x, y, x + y.toInt<SizeT>()));
	auto sum = 0Sz;
	for (auto i : list) sum += i.toInt<SizeT>();
	Asserts::AssertEqual(res, sum);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::AnyEmptyList::Excecute() {
	Asserts::AssertFalse(Core::Containers::ConcurencyList<SizeT>().Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::AnyWithItemList::Excecute() {
	Asserts::AssertTrue(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::AnyWithConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 5)));
	Asserts::AssertFalse(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).Any(ArrowFunct(x, x > 10)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::AllWithConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).All(ArrowFunct(x, x >= 0)));
	Asserts::AssertFalse(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).All(ArrowFunct(x, x < 8)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::CloneListWithPods::Excecute() {
	auto list = Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10);
	auto clone = list.Clone();
	Asserts::AssertEqual(list.Count, clone.Count);
	Asserts::AssertTrue([&] () InlineLambda
	{
		auto listIt = list.FirstIt;
		auto cloneIt = list.FirstIt;
		while (listIt != nullptr && cloneIt != nullptr)
		{
			if ((*listIt) != (*cloneIt))
				return false;

			++listIt; ++cloneIt;
		}
		return !(list.IsSameAs(clone));
	});
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::CloneListWithClonableItems::Excecute() {
	auto list = Core::Containers::ConcurencyList<Core::Containers::ConcurencyList<String>>::Generate(
				ArrowFunct(
					Core::Containers::ConcurencyList<String>::Generate(
							ArrowFunct(x, String::toString(x)), 
					10)
				), 
				10);

	auto clone = list.Clone();
	Asserts::AssertEqual(list.Count, clone.Count);
	Asserts::AssertEqual(list.Aggregate<SizeT>(ArrowFunct(x, y, x + y.Count)), clone.Aggregate<SizeT>(ArrowFunct(x, y, x + y.Count)));
	Asserts::AssertEqual(
		list.Aggregate<String>(ArrowFunct(x, y, x + y.Aggregate(ArrowFunct(x, y, x + y)))),
		clone.Aggregate<String>(ArrowFunct(x, y, x + y.Aggregate(ArrowFunct(x, y, x + y)))));

	return !(list.IsSameAs(clone));
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::FirstConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, x >= 5)) == 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::LastConditionList::Excecute() {
	Asserts::AssertTrue(Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, x <= 5)) == 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::ListIteratorsLoop::Excecute() {
	auto list = Core::Containers::ConcurencyList<SizeT>().Generate(ArrowFunct(i, i), 10);
	
	Asserts::AssertTrue([&] () InlineLambda
	{
		auto i = 0;
		for (auto& j : list)
		{
			if (i != j)
				return false;
			i++;
		}

		return true;
	});

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::MaxItem::Excecute() {
	return Core::Containers::ConcurencyList<SizeT>::Generate(ArrowFunct(i, i), 10).Max() == 9;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::MaxItemField::Excecute() {
	return Core::Containers::ConcurencyList<String>({ L"1"S, L"12"S, L"123"S }).Max(ArrowFunct(x, x.Lenght)) == 3;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::MaxByItemField::Excecute() {
	return Core::Containers::ConcurencyList<String>({ L"1"S, L"12"S, L"123"S }).MaxBy(ArrowFunct(x, x.Lenght)) == L"123";
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::MinItem::Excecute() {
	return Core::Containers::ConcurencyList<SizeT>::Generate(ArrowFunct(i, i), 10).Min() == 0;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::MinItemField::Excecute() {
	return Core::Containers::ConcurencyList<String>({ L"1"S, L"12"S, L"123"S }).Min(ArrowFunct(x, x.Lenght)) == 1;
}

bool Seed::Core::Testing::Test::API::Containers::List::Concurency::MinByItemField::Excecute() {
	return Core::Containers::ConcurencyList<String>({ L"1"S, L"12"S, L"123"S }).MinBy(ArrowFunct(x, x.Lenght)) == L"1";
}

#pragma endregion

#pragma region MultiThreadConcurency

bool Seed::Core::Testing::Test::API::Containers::List::MultiThreadConcurency::IteratorsLoop::Excecute() {
	auto sharedList = Core::Containers::ConcurencyList<String>::Generate(ArrowFunct(i, String::toString(i)), 1000Sz);

	auto threads = Core::Containers::List<Core::Threading::Thread>::Generate(
		ArrowFunctWithClosure(i, 
			Core::Threading::Thread(L"Multi thread list iterators loop test thread " + String::toString(i), Delegate([&] ()
				{
					int i = 0;
					for (auto& item : sharedList)
					{
						Asserts::AssertEqual(item.toInt(), i);
						i++;
					}

					return i;
				})
			)
		), Core::CurrentSettings().VirtualSetting.CpuCoresCount)
		.ForEach(ArrowFunct(thread, thread.Start()))
		.ForEach(ArrowFunct(thread, thread.Join()))
		.ForEach([&] (Core::Threading::Thread& thread)
		{
			Asserts::AssertEqual(thread.GetThreadResult<int>(), sharedList.Count);
		});

	return true;
}

#pragma endregion