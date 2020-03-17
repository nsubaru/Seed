#include "Seed.Core.Containers.NonConcurency.FixedCircularBuffer.h"
#include "Seed.Core.Containers.Concurency.FixedCircularBuffer.h"

#include "Seed.Core.Testing.Tests.Containers.FixedCircularBuffer.h"

bool Seed::Core::Testing::Test::Containers::FixedCircularBuffer::Excecute() {
	auto buffer = Core::Containers::FixedCircularBuffer<Int32, 10>();

	Asserts::AssertEqual(buffer.Aggregate([&] (Int32& r, const Int32& v)
	{
		r += v;
	}, 0), 0);

	for (SizeT i = 0; i < 5; i++)
	{
		buffer += 1;
	}

	Asserts::AssertEqual(
		buffer.Aggregate([&] (Int32& r, const Int32& v)
		{
			r += v;
		}, 0), 
	5);

	for (SizeT i = 0; i < 10; i++)
	{
		buffer += 1;
	}

	Asserts::AssertEqual(
		buffer.Aggregate([&] (Int32& r, const Int32& v)
		{
			r += v;
		}, 0), 
	10);

	return true;
}

bool Seed::Core::Testing::Test::Containers::ConcurencyFixedCircularBuffer::Excecute() {
	auto buffer = Core::Containers::FixedConcurencyCircularBuffer<Int32, 10>();

	Asserts::AssertEqual(buffer.Aggregate([&] (Int32& r, const Int32& v)
	{
		r += v;
	}, 0), 0);

	for (SizeT i = 0; i < 5; i++)
	{
		buffer += 1;
	}

	Asserts::AssertEqual(
		buffer.Aggregate([&] (Int32& r, const Int32& v)
		{
			r += v;
		}, 0), 
	5);

	for (SizeT i = 0; i < 10; i++)
	{
		buffer += 1;
	}

	Asserts::AssertEqual(
		buffer.Aggregate([&] (Int32& r, const Int32& v)
		{
			r += v;
		}, 0), 
	10);

	return true;
}
