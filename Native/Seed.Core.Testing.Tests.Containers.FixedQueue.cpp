// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Containers.FixedQueue.h"
#include "Seed.Core.Containers.Concurency.FixedQueue.h"
#include "Seed.Core.Containers.NonConcurency.FixedQueue.h"

bool Seed::Testing::Test::Containers::SimpleTypeFixedQueueTest::Excecute()
{
	Core::Containers::FixedQueue<10, Int32> queue{ 1,2,3,4 };
	auto Sum = 0;
    for (auto i = 11; i < 15; i++)
    {
        queue.Enqueue(i); Sum += i;
    }
	auto IsDequeueWork = true;
    for (auto i = 1; i < 5 ; i++)
    {
	    const auto value = queue.Dequeue();
        if (IsDequeueWork)
            if (value != i)
                IsDequeueWork = false;
    }
    if (!IsDequeueWork)
        return false;
    for (auto i = 15; i < 19; i++)
    {
        queue.Enqueue(i);
        Sum += i;
    }
	const auto IsAnyWork = !queue.Any([](const Int32& item) InlineLambda {return item > 19; });
	const auto IsAllWork = queue.All([](const Int32& item) InlineLambda {return item > 10; });
	const auto IsAggregateWork = queue.Aggregate([](Int32& result, const Int32& item) InlineLambda {result += item; }) == Sum;
    return IsAnyWork && IsAllWork && IsAggregateWork;
}

bool Seed::Testing::Test::Containers::SimpleTypeConcurencyFixedQueueTest::Excecute()
{
	Core::Containers::ConcurencyFixedQueue<10, Int32> queue{ 1,2,3,4 };
	auto Sum = 0;
    for (auto i = 11; i < 15; i++)
    {
        queue.Enqueue(i); Sum += i;
    }
	auto IsDequeueWork = true;
    for (auto i = 1; i < 5 ; i++)
    {
	    const auto value = queue.Dequeue();
        if (IsDequeueWork)
            if (value != i)
                IsDequeueWork = false;
    }
    if (!IsDequeueWork)
        return false;
    for (auto i = 15; i < 19; i++)
    {
        queue.Enqueue(i);
        Sum += i;
    }
	const auto IsAnyWork = !queue.Any([](const Int32& item) InlineLambda {return item > 19; });
	const auto IsAllWork = queue.All([](const Int32& item) InlineLambda {return item > 10; });
	const auto IsAggregateWork = queue.Aggregate([](Int32& result, const Int32& item) InlineLambda {result += item; }) == Sum;
    return IsAnyWork && IsAllWork && IsAggregateWork;
}

bool Seed::Testing::Test::Containers::ComplexTypeFixedQueueTest::Excecute()
{
	Core::Containers::FixedQueue<10, String> queue{L"1", L"2", L"3", L"4"};
	auto Sum = 0;
    for (auto i = 11; i < 15; i++)
    {
        queue.Enqueue(String::toString(i)); Sum += i;
    }
	auto IsDequeueWork = true;
    for (auto i = 1; i < 5 ; i++)
    {
	    const auto value = queue.Dequeue();
        if (IsDequeueWork)
            if (value != String::toString(i))
                IsDequeueWork = false;
    }
    if (!IsDequeueWork)
        return false;
    for (auto i = 15; i < 19; i++)
    {
        queue.Enqueue(String::toString(i));
        Sum += i;
    }
	const auto IsAnyWork = !queue.Any([](const String& item) InlineLambda {return item.toInt() > 19; });
	const auto IsAllWork = queue.All([](const String& item) InlineLambda {return item.toInt() > 10; });
	const auto IsAggregateWork = queue.Aggregate([](Int32& result, const String& item) InlineLambda {result += item.toInt(); }, 0) == Sum;
    return IsAnyWork && IsAllWork && IsAggregateWork;
}

bool Seed::Testing::Test::Containers::ComplexTypeConcurencyFixedQueueTest::Excecute()
{
	Core::Containers::ConcurencyFixedQueue<10, String> queue{L"1", L"2", L"3", L"4"};
	auto Sum = 0;
    for (auto i = 11; i < 15; i++)
    {
        queue.Enqueue(String::toString(i)); Sum += i;
    }
	auto IsDequeueWork = true;
    for (auto i = 1; i < 5 ; i++)
    {
	    const auto value = queue.Dequeue();
        if (IsDequeueWork)
            if (value != String::toString(i))
                IsDequeueWork = false;
    }
    if (!IsDequeueWork)
        return false;
    for (auto i = 15; i < 19; i++)
    {
        queue.Enqueue(String::toString(i));
        Sum += i;
    }
	const auto IsAnyWork = !queue.Any([](const String& item) InlineLambda {return item.toInt() > 19; });
	const auto IsAllWork = queue.All([](const String& item) InlineLambda {return item.toInt() > 10; });
	const auto IsAggregateWork = queue.Aggregate([](Int32& result, const String& item) InlineLambda {result += item.toInt(); }, 0) == Sum;
    return IsAnyWork && IsAllWork && IsAggregateWork;
}
