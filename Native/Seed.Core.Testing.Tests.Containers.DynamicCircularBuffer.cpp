#include "Seed.Core.Containers.NonConcurency.DynamicCircularBuffer.h"
#include "Seed.Core.Containers.Concurency.DynamicCircularBuffer.h"

#include "Seed.Core.Testing.Tests.Containers.DynamicCircularBuffer.h"

bool Seed::Core::Testing::Test::Containers::DynamicCircularBuffer::Excecute() {
	auto buffer = Core::Containers::DynamicCircularBuffer<Int32>(10);

	for (Int32 i = 0; i < 5; i++)
	{
		buffer += 1;
	}

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 5;
	});

	for (Int32 i = 0; i < 10; i++)
	{
		buffer += 1;
	}

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 10;
	});

	buffer.Resize(20);

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 10;
	});

	for (Int32 i = 0; i < 20; i++)
	{
		buffer += 1;
	}

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 20;
	});

	return true;
}

bool Seed::Core::Testing::Test::Containers::ConcurencyDynamicCircularBuffer::Excecute() {
	auto buffer = Core::Containers::ConcurencyDynamicCircularBuffer<Int32>(10);

	for (Int32 i = 0; i < 5; i++)
	{
		buffer += 1;
	}

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 5;
	});

	for (Int32 i = 0; i < 10; i++)
	{
		buffer += 1;
	}

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 10;
	});

	buffer.Resize(20);

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 10;
	});

	for (Int32 i = 0; i < 20; i++)
	{
		buffer += 1;
	}

	Asserts::AssertTrue([&] ()
	{
		return buffer.Aggregate([] (Int32& res, const Int32& item)
		{
			res += item;
		}, 0) == 20;
	});

	return true;
}
