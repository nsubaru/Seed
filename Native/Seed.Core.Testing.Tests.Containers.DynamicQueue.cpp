#include "Seed.Core.Threading.h"

#include "Seed.Core.Testing.Tests.Containers.DynamicQueue.h"

#pragma region NonConcurency

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::ForEachTestDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	SizeT sum = 0;
	items.ForEach(ArrowFunctWithClosure(x, sum += x));
	Asserts::AssertEqual(sum, 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	items.ForEach(ArrowFunct(x, y, Asserts::AssertEqual(x, y)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::ForEachToTestDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	SizeT sum = 0;
	items.ForEach(ArrowFunctWithClosure(x, sum += x), 5);
	Asserts::AssertEqual(sum, 1 + 2 + 3 + 4);
	items.ForEach(ArrowFunct(x, y, Asserts::AssertEqual(x, y)), 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::AgregateSimpleItemsTypeTestDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertEqual(items.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::AgregateConvertionsItemsTypeTestDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<String>::Generate(ArrowFunct(x, String::toString(x)), 10);
	Asserts::AssertEqual(items.Aggregate<Int32>(ArrowFunct(x, y, x + y.toInt())), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::AnyEmptyDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>();
	Asserts::AssertFalse(items.Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::AnyWithItemDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::AnyWithConditionDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.Any(ArrowFunct(x, x > 5)));
	Asserts::AssertFalse(items.Any(ArrowFunct(x, x > 10)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::AllWithConditionDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.All(ArrowFunct(x, x >= 0)));
	Asserts::AssertFalse(items.All(ArrowFunct(x, x > 5)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::CloneDynamicQueueWithPods::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	auto clone = items.Clone();
	Asserts::AssertEqual(items.Count, clone.Count);
	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < items.Count; i++)
		{
			if (items.Dequeue() != clone.Dequeue())
				return false;
		}
		return !(items.IsSameAs(clone));
	});
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::CloneDynamicQueueWithClonableItems::Excecute() {
	auto items = Core::Containers::DynamicQueue<String>::Generate(ArrowFunct(x, String::toString(x)), 10);
	auto clone = items.Clone();
	Asserts::AssertEqual(items.Count, clone.Count);
	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < items.Count; i++)
		{
			if (items.Dequeue() != clone.Dequeue())
				return false;
		}
		return !(items.IsSameAs(clone));
	});
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::ShrinkDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	for (SizeT i = 0; i < 5; i++)
	{
		[[maybe_unused]] auto _ = items.Dequeue();
	}
	Asserts::AssertEqual(items.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 5; i++) x += 5 + i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::ExtendDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.IsFull);
	Asserts::AssertThrowException<FullQueueException>([&] ()
	{
		items.Enqueue(1);
	});
	items.Extend(1);
	Asserts::AssertFalse(items.IsFull);
	Asserts::AssertNoThrowException<FullQueueException>([&] ()
	{
		items.Enqueue(1);
	});
	Asserts::AssertTrue(items.IsFull);
	Asserts::AssertEqual(items.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x + 1;
	}());

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::FirstConditionDynamicQueue::Excecute() {
	return Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, true)) == 0;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::LastConditionDynamicQueue::Excecute() {
	return Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, true)) == 9;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::EqueuDynamicQueue::Excecute() {
	auto queue = Core::Containers::DynamicQueue<SizeT>(10);
	for (SizeT i = 0; i < queue.MaxCount; i++)
	{
		queue.Enqueue(i);
	}

	Asserts::AssertEqual(queue.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::DequeuDynamicQueue::Excecute() {
	auto queue = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	for (SizeT i = 0; i < queue.Count; i++)
	{
		Asserts::AssertEqual(queue.Dequeue(), i);
	}

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::NonConcurency::TopDynamicQueue::Excecute() {
	auto queue = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	for (SizeT i = 0; i < queue.Count; i++)
	{
		Asserts::AssertEqual(queue.Top(), i);
		[[maybe_unused]] auto _ = queue.Dequeue();
	}

	return true;
}

#pragma endregion

#pragma region Concurency
bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::ForEachTestDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	SizeT sum = 0;
	items.ForEach(ArrowFunctWithClosure(x, sum += x));
	Asserts::AssertEqual(sum, 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9);
	items.ForEach(ArrowFunct(x, y, Asserts::AssertEqual(x, y)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::ForEachToTestDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	SizeT sum = 0;
	items.ForEach(ArrowFunctWithClosure(x, sum += x), 5);
	Asserts::AssertEqual(sum, 1 + 2 + 3 + 4);
	items.ForEach(ArrowFunct(x, y, Asserts::AssertEqual(x, y)), 5);
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::AgregateSimpleItemsTypeTestDynamicQueue::Excecute() {
	auto items = Core::Containers::DynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertEqual(items.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::AgregateConvertionsItemsTypeTestDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<String>::Generate(ArrowFunct(x, String::toString(x)), 10);
	Asserts::AssertEqual(items.Aggregate<Int32>(ArrowFunct(x, y, x + y.toInt())), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::AnyEmptyDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>();
	Asserts::AssertFalse(items.Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::AnyWithItemDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.Any());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::AnyWithConditionDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.Any(ArrowFunct(x, x > 5)));
	Asserts::AssertFalse(items.Any(ArrowFunct(x, x > 10)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::AllWithConditionDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.All(ArrowFunct(x, x >= 0)));
	Asserts::AssertFalse(items.All(ArrowFunct(x, x > 5)));
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::CloneDynamicQueueWithPods::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	auto clone = items.Clone();
	Asserts::AssertEqual(items.Count, clone.Count);
	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < items.Count; i++)
		{
			if (items.Dequeue() != clone.Dequeue())
				return false;
		}
		return !(items.IsSameAs(clone));
	});
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::CloneDynamicQueueWithClonableItems::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<String>::Generate(ArrowFunct(x, String::toString(x)), 10);
	auto clone = items.Clone();
	Asserts::AssertEqual(items.Count, clone.Count);
	Asserts::AssertTrue([&] ()
	{
		for (SizeT i = 0; i < items.Count; i++)
		{
			if (items.Dequeue() != clone.Dequeue())
				return false;
		}
		return !(items.IsSameAs(clone));
	});
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::ShrinkDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	for (SizeT i = 0; i < 5; i++)
	{
		[[maybe_unused]] auto _ = items.Dequeue();
	}
	Asserts::AssertEqual(items.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 5; i++) x += 5 + i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::ExtendDynamicQueue::Excecute() {
	auto items = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	Asserts::AssertTrue(items.IsFull);
	items.Extend(1);
	Asserts::AssertFalse(items.IsFull);
	Asserts::AssertNoThrowException<FullQueueException>([&] ()
	{
		items.Enqueue(1);
	});
	Asserts::AssertTrue(items.IsFull);
	Asserts::AssertEqual(items.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x + 1;
	}());

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::FirstConditionDynamicQueue::Excecute() {
	return Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(i, i), 10).First(ArrowFunct(x, true)) == 0;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::LastConditionDynamicQueue::Excecute() {
	return Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(i, i), 10).Last(ArrowFunct(x, true)) == 9;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::EqueuDynamicQueue::Excecute() {
	auto queue = Core::Containers::ConcurencyDynamicQueue<SizeT>(10);
	for (SizeT i = 0; i < queue.MaxCount; i++)
	{
		queue.Enqueue(i);
	}

	Asserts::AssertEqual(queue.Aggregate(ArrowFunct(x, y, x + y)), [] ()
	{
		SizeT x = 0; for (SizeT i = 0; i < 10; i++) x += i;
		return x;
	}());
	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::DequeuDynamicQueue::Excecute() {
	auto queue = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	for (SizeT i = 0; i < queue.Count; i++)
	{
		Asserts::AssertEqual(queue.Dequeue(), i);
	}

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::TopDynamicQueue::Excecute() {
	auto queue = Core::Containers::ConcurencyDynamicQueue<SizeT>::Generate(ArrowFunct(x, x), 10);
	for (SizeT i = 0; i < queue.Count; i++)
	{
		Asserts::AssertEqual(queue.Top(), i);
		[[maybe_unused]] auto _ = queue.Dequeue();
	}

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::DynamicQueue::Concurency::MultiThreadQueueProcessingDynamicQueue::Excecute() {
	constexpr auto queueSize = 10Kb;

	auto queue = Core::Containers::ConcurencyDynamicQueue<SizeT>(queueSize);
	
	auto genThread = Core::Threading::Thread(L"Multi thread dynamic queue enqueue thread", Delegate([] (Core::Containers::ConcurencyDynamicQueue<SizeT>& queue)
	{
		for (SizeT i = 0; i < queueSize; i++)
		{
			queue.Enqueue(i);
		}
		return true;
	}, queue));

	auto processing = Core::Threading::Thread(L"Multi thread dynamic queue de thread", Delegate([] (Core::Containers::ConcurencyDynamicQueue<SizeT>& queue)
	{
		bool IsAllDequeue = true;
		for (SizeT i = 0; i < queueSize; i++)
		{
			if (i != queue.Dequeue())
				IsAllDequeue = false;
		}
		Asserts::AssertTrue(IsAllDequeue);
		return true;
	}, queue));

	genThread.Start();
	processing.Start();
	genThread.Join();
	processing.Join();

	Asserts::AssertTrue(genThread.GetThreadResult<Boolean>());
	Asserts::AssertTrue(processing.GetThreadResult<Boolean>());

	return true;
}
#pragma endregion