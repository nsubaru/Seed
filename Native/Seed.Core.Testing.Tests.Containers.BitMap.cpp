#include "Seed.Core.Testing.Tests.Containers.BitMap.h"

#include "Seed.Core.Containers.BitMap.h"
#include "Seed.Core.Containers.BitMapImpl.h"

bool Seed::Core::Testing::Test::API::Containers::BitMap::DefaultConstructorInitializesBitsAsZeroTest::Execute() {
	Core::Containers::BitMapT<17> map;

	Asserts::AssertEqual(map.Count, (SizeT) 17, SourcePosition);
	Asserts::AssertEqual(map.SetBitCount(), (SizeT) 0, SourcePosition);

	for (SizeT i = 0; i < map.Count; i++)
	{
		Asserts::AssertTrue(map.IsZero(i), SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::SetAllConstructorSetsOnlyActualBitsTest::Execute() {
	Core::Containers::BitMapT<10> map(true);

	Asserts::AssertEqual(map.SetBitCount(), (SizeT) 10, SourcePosition);
	Asserts::AssertEqual(map[0], (Byte) 0b11111111, SourcePosition);
	Asserts::AssertEqual(map[1], (Byte) 0b00000011, SourcePosition);

	for (SizeT i = 0; i < map.Count; i++)
	{
		Asserts::AssertFalse(map.IsZero(i), SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::SetResetBitAtClusterBoundaryTest::Execute() {
	Core::Containers::BitMapT<17> map;

	map.SetBit(7);
	map.SetBit(8);
	map.SetBit(16);

	Asserts::AssertFalse(map.IsZero(7), SourcePosition);
	Asserts::AssertFalse(map.IsZero(8), SourcePosition);
	Asserts::AssertFalse(map.IsZero(16), SourcePosition);
	Asserts::AssertEqual(map.SetBitCount(), (SizeT) 3, SourcePosition);

	map.ResetBit(8);

	Asserts::AssertFalse(map.IsZero(7), SourcePosition);
	Asserts::AssertTrue(map.IsZero(8), SourcePosition);
	Asserts::AssertFalse(map.IsZero(16), SourcePosition);
	Asserts::AssertEqual(map.SetBitCount(), (SizeT) 2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::SetResetBitSequelAcrossClustersTest::Execute() {
	Core::Containers::BitMapT<24> map;

	map.SetBitSequel(5, 14);
	Asserts::AssertEqual(map.SetBitCount(), (SizeT) 14, SourcePosition);

	for (SizeT i = 0; i < map.Count; i++)
	{
		Asserts::AssertEqual(map.IsZero(i), !(i >= 5 && i < 19), SourcePosition);
	}

	map.ResetBitSequel(7, 9);
	Asserts::AssertEqual(map.SetBitCount(), (SizeT) 5, SourcePosition);

	for (SizeT i = 0; i < map.Count; i++)
	{
		const Boolean expectedIsSet = (i >= 5 && i < 7) || (i >= 16 && i < 19);
		Asserts::AssertEqual(map.IsZero(i), !expectedIsSet, SourcePosition);
	}

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::FindBitSequelSkipsOccupiedRangesTest::Execute() {
	Core::Containers::BitMapT<16> map;

	map.SetBitSequel(0, 3);
	map.SetBitSequel(5, 4);

	Asserts::AssertEqual(map.FindBitSequel(2), (SizeT) 3, SourcePosition);
	Asserts::AssertEqual(map.FindBitSequel(4), (SizeT) 9, SourcePosition);

	map.SetBitSequel(3, 2);
	map.SetBitSequel(9, 7);

	Asserts::AssertEqual(map.FindBitSequel(1), Math::Max<SizeT>(), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::FindBitSequalFromHonorsStartAndBoundsTest::Execute() {
	Core::Containers::BitMapT<20> map;

	map.SetBitSequel(0, 10);
	map.SetBitSequel(14, 2);

	Asserts::AssertEqual(map.FindBitSequalFrom(0, 3), (SizeT) 10, SourcePosition);
	Asserts::AssertEqual(map.FindBitSequalFrom(12, 3), (SizeT) 16, SourcePosition);
	Asserts::AssertEqual(map.FindBitSequalFrom(18, 3), Math::Max<SizeT>(), SourcePosition);
	Asserts::AssertEqual(map.FindBitSequalFrom(20, 1), Math::Max<SizeT>(), SourcePosition);
	Asserts::AssertEqual(map.FindBitSequalFrom(5, 0), (SizeT) 0, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::BitwiseOperationsRespectPartialClusterTest::Execute() {
	Core::Containers::BitMapT<10> first;
	Core::Containers::BitMapT<10> second;

	first.SetBit(0);
	first.SetBit(8);
	second.SetBit(8);
	second.SetBit(9);

	auto orMap = first;
	orMap |= second;

	Asserts::AssertEqual(orMap.SetBitCount(), (SizeT) 3, SourcePosition);
	Asserts::AssertFalse(orMap.IsZero(0), SourcePosition);
	Asserts::AssertFalse(orMap.IsZero(8), SourcePosition);
	Asserts::AssertFalse(orMap.IsZero(9), SourcePosition);

	auto andMap = first;
	andMap &= second;

	Asserts::AssertEqual(andMap.SetBitCount(), (SizeT) 1, SourcePosition);
	Asserts::AssertTrue(andMap.IsZero(0), SourcePosition);
	Asserts::AssertFalse(andMap.IsZero(8), SourcePosition);
	Asserts::AssertTrue(andMap.IsZero(9), SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::EqualityIgnoresPaddingBitsTest::Execute() {
	Core::Containers::BitMapT<10> first;
	Core::Containers::BitMapT<10> second;

	first.SetBit(0);
	first.SetBit(9);
	second.SetBit(0);
	second.SetBit(9);
	second[1] |= (Byte) 0b11111100;

	Asserts::AssertEqual(second.SetBitCount(), (SizeT) 2, SourcePosition);
	Asserts::AssertTrue(first == second, SourcePosition);
	Asserts::AssertFalse(first != second, SourcePosition);

	second.ResetBit(9);

	Asserts::AssertFalse(first == second, SourcePosition);
	Asserts::AssertTrue(first != second, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::API::Containers::BitMap::OutOfRangeAccessThrowsTest::Execute() {
	Core::Containers::BitMapT<8> map;

	Asserts::AssertThrowException<IndexOutOfRange>([&]() InlineLambda
	{
		map.SetBit(8);
	}, SourcePosition);

	Asserts::AssertThrowException<IndexOutOfRange>([&]() InlineLambda
	{
		map.ResetBit(8);
	}, SourcePosition);

	Asserts::AssertThrowException<IndexOutOfRange>([&]() InlineLambda
	{
		map.IsZero(8);
	}, SourcePosition);

	Asserts::AssertThrowException<IndexOutOfRange>([&]() InlineLambda
	{
		map.SetBitSequel(4, 5);
	}, SourcePosition);

	Asserts::AssertThrowException<IndexOutOfRange>([&]() InlineLambda
	{
		map.ResetBitSequel(7, 2);
	}, SourcePosition);

	Asserts::AssertThrowException<IndexOutOfRange>([&]() InlineLambda
	{
		map[1];
	}, SourcePosition);

	return true;
}
