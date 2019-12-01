// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Containers.PriorityQueue.h"
#include "Seed.Core.Containers.Concurency.PriorityQueue.h"
#include "Seed.Core.Containers.NonConcurency.PriorityQueue.h"

bool Seed::Core::Testing::Test::Containers::SimpleTypePriorityQueueTest::Excecute()
{
	auto queue = Seed::Containers::PriorityQueue<Int32, Int32>{ {1,1},{2,1},{2,2},{3,1},{3,2},{3,3},{4,4} };
	bool IsForEachKeyWorkCorrect = true;
	bool IsForEachValueWorkCorrect = true;

	auto copy = 
		queue.ForEachKey([&](const Int32& key) { IsForEachKeyWorkCorrect = (IsForEachKeyWorkCorrect) ? key < 5 : IsForEachKeyWorkCorrect; })
			.ForEachValue([&](const Int32& value) {IsForEachValueWorkCorrect = (IsForEachValueWorkCorrect) ? value < 5 : IsForEachValueWorkCorrect; })
			.Clone();

	bool IsCopyValid = (queue.Count == copy.Count);

	if (IsCopyValid) {
		while (!queue.IsEmpty) {
			bool state = queue.Dequeu() == copy.Dequeu();
			IsCopyValid = (IsCopyValid) ?  state : IsCopyValid;
		}
	}

	return IsForEachKeyWorkCorrect && IsForEachValueWorkCorrect && IsCopyValid;
}

bool Seed::Core::Testing::Test::Containers::ComplexTypePriorityQueueTest::Excecute()
{
	auto queue = Seed::Containers::PriorityQueue<Int32, String>{ {1,L"1"},{2,L"1"},{2,L"2"},{3,L"1"},{3,L"2"},{3,L"3"},{4,L"4"} };
	bool IsForEachKeyWorkCorrect = true;
	bool IsForEachValueWorkCorrect = true;

	auto copy =
		queue.ForEachKey([&](const Int32& key) { IsForEachKeyWorkCorrect = (IsForEachKeyWorkCorrect) ? key < 5 : IsForEachKeyWorkCorrect; })
		.ForEachValue([&](const String& value) {IsForEachValueWorkCorrect = (IsForEachValueWorkCorrect) ? value.toInt() < 5 : IsForEachValueWorkCorrect; })
		.Clone();

	bool IsCopyValid = (queue.Count == copy.Count);

	if (IsCopyValid) {
		while (!queue.IsEmpty) {
			bool state = queue.Dequeu() == copy.Dequeu();
			IsCopyValid = (IsCopyValid) ? state : IsCopyValid;
		}
	}

	return IsForEachKeyWorkCorrect && IsForEachValueWorkCorrect && IsCopyValid;
}

bool Seed::Core::Testing::Test::Containers::SimpleTypeConcurencyPriorityQueueTest::Excecute()
{
	auto queue = Seed::Containers::ConcurencyPriorityQueue<Int32, Int32>{ {1,1},{2,1},{2,2},{3,1},{3,2},{3,3},{4,4} };
	bool IsForEachKeyWorkCorrect = true;
	bool IsForEachValueWorkCorrect = true;

	auto copy =
		queue.ForEachKey([&](const Int32& key) { IsForEachKeyWorkCorrect = (IsForEachKeyWorkCorrect) ? key < 5 : IsForEachKeyWorkCorrect; })
		.ForEachValue([&](const Int32& value) {IsForEachValueWorkCorrect = (IsForEachValueWorkCorrect) ? value < 5 : IsForEachValueWorkCorrect; })
		.Clone();

	bool IsCopyValid = (queue.Count == copy.Count);

	if (IsCopyValid) {
		while (!queue.IsEmpty) {
			bool state = queue.Dequeu() == copy.Dequeu();
			IsCopyValid = (IsCopyValid) ? state : IsCopyValid;
		}
	}

	return IsForEachKeyWorkCorrect && IsForEachValueWorkCorrect && IsCopyValid;
}

bool Seed::Core::Testing::Test::Containers::ComplexTypeConcurencyPriorityQueueTest::Excecute()
{
	auto queue = Seed::Containers::ConcurencyPriorityQueue<Int32, String>{ {1,L"1"},{2,L"1"},{2,L"2"},{3,L"1"},{3,L"2"},{3,L"3"},{4,L"4"} };
	bool IsForEachKeyWorkCorrect = true;
	bool IsForEachValueWorkCorrect = true;

	auto copy =
		queue.ForEachKey([&](const Int32& key) { IsForEachKeyWorkCorrect = (IsForEachKeyWorkCorrect) ? key < 5 : IsForEachKeyWorkCorrect; })
		.ForEachValue([&](const String& value) {IsForEachValueWorkCorrect = (IsForEachValueWorkCorrect) ? value.toInt() < 5 : IsForEachValueWorkCorrect; })
		.Clone();

	bool IsCopyValid = (queue.Count == copy.Count);

	if (IsCopyValid) {
		while (!queue.IsEmpty) {
			bool state = queue.Dequeu() == copy.Dequeu();
			IsCopyValid = (IsCopyValid) ? state : IsCopyValid;
		}
	}

	return IsForEachKeyWorkCorrect && IsForEachValueWorkCorrect && IsCopyValid;
}
