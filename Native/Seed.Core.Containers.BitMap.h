#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		//Бітова карта незміного розміру, кількість бітів вказується аргументом BitCount
		//BitCount не може бути більший ніж (2 * 1024 * 1024 * 1024 * 8) - 8, тобто 2Гб - 1 байт
		template<SizeT BitCount>
		class BitMapT {
			static_assert(BitCount != 0, "BitMap bit count must not equal zero");
			static_assert(Seed::Math::RoundToUpper(BitCount, sizeof(Byte) * 8ui64) < 2Gb, "BitMap size must not bigger then 2GB - 1b");
		private:
			static constexpr SizeT size = Seed::Math::RoundToUpper(BitCount, sizeof(Byte) * 8ui64);
			static constexpr SizeT fullBytesCount = BitCount / 8ui64;
			static constexpr SizeT LastByteBitsCount = BitCount - (fullBytesCount * 8);
		public:
			__forceinline constexpr SizeT GetCount() { return BitCount; }
			SizeT Property(GetCount) Count;
		private:
			Byte bitMap[size] = { 0 };
		public:
			__forceinline constexpr BitMapT(Boolean IsSetAll = false) {
				if (IsSetAll) {
					for (SizeT i = 0; i < fullBytesCount; i++)
					{
						bitMap[i] = Seed::Math::Max<Byte>();
					}
					if constexpr (fullBytesCount < size) {
						for (SizeT i = 0; i < LastByteBitsCount; i++)
						{
							BitSet(bitMap[fullBytesCount], i);
						}
					}
				}
			}

			constexpr SizeT FindBitSequal(SizeT RequireCount) {
				SizeT RangeSize;
				Boolean IsFound = false;
				for (SizeT byte = 0; byte < fullBytesCount; byte++) {
					if (bitMap[byte] == Math::Max<Byte>())
						continue;
					for (SizeT bit = 0; bit < 8ui64; bit++) {
						if (!BitCheck(bitMap[byte], bit))
							if (IsFound == false) {
								RangeSize = 0; IsFound = true;
							}
							else {
								RangeSize++;
								if (RangeSize == RequireCount)
									return ((byte * 8) + bit) - RequireCount;
							}
						else
							if (IsFound == true)
								IsFound = false;
					}
				}
				if constexpr (fullBytesCount < size)
					for (SizeT bit = 0; bit < 8ui64; bit++) {
						if (!BitCheck(bitMap[fullBytesCount], bit))
							if (IsFound == false) {
								RangeSize = 0; IsFound = true;
							}
							else {
								RangeSize++;
								if (RangeSize == RequireCount)
									return ((fullBytesCount * 8) + bit) - RequireCount;
							}
						else
							if (IsFound == true)
								IsFound = false;
					}
				return -1;
			};

			constexpr void SetBitSequal(SizeT Position, SizeT Count) {
				if (Position + Count > BitCount)
					Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount + Count));

				SizeT StartByte = Position / 8;
				SizeT StartBit = Position % 8;

				SizeT EndByte = ((Position / 8) + Count);
				SizeT EndBit = ((Position % 8) + Count) % 8;

				EndBit++;

				if (StartByte == EndByte) {
					for (SizeT bit = StartBit; bit < EndBit; bit++)
						BitSet(bitMap[StartByte], bit);
				}
				else {
					if (StartBit != 0) {
						for (SizeT bit = StartBit; bit < 8ui64; bit++)
							BitSet(bitMap[StartByte], bit);
					}
					else {
						bitMap[StartByte] = Math::Max<Byte>();
					}
					if (EndBit != 8) {
						for (SizeT bit = 0; bit < EndBit; bit++)
							BitSet(bitMap[EndByte], bit);
					}
					else {
						bitMap[EndByte] = Math::Max<Byte>();
					}
					if (StartByte - EndByte > 1) {
						for (SizeT byte = StartByte + 1; byte < EndByte; byte++)
							bitMap[byte] = Math::Max::Byte;
					}
				}
			};
			constexpr void ResetBitSequal(SizeT Position, SizeT Count) {
				if (Position + Count > BitCount)
					Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount + Count));

				SizeT StartByte = Position / 8;
				SizeT StartBit = Position % 8;

				SizeT EndByte = ((Position / 8) + Count);
				SizeT EndBit = ((Position % 8) + Count) % 8;

				EndBit++;

				if (StartByte == EndByte) {
					for (SizeT bit = StartBit; bit < EndBit; bit++)
						BitReset(bitMap[StartByte], bit);
				}
				else {
					if (StartBit != 0) {
						for (SizeT bit = StartBit; bit < 8ui64; bit++)
							BitReset(bitMap[StartByte], bit);
					}
					else {
						bitMap[StartByte] = 0;
					}
					if (EndBit != 8) {
						for (SizeT bit = 0; bit < EndBit; bit++)
							BitReset(bitMap[EndByte], bit);
					}
					else {
						bitMap[EndByte] = 0;
					}
					if (StartByte - EndByte > 1) {
						for (SizeT byte = StartByte + 1; byte < EndByte; byte++)
							bitMap[byte] = 0;
					}
				}
			};

			constexpr void SetBit(SizeT Position) {
				if (Position > BitCount)
					Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
				BitSet(bitMap[Position / 8ui64], Position % 8ui64);
			};

			constexpr void ResetBit(SizeT Position) {
				if (Position > BitCount)
					Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
				BitReset(bitMap[Position / 8ui64], Position % 8ui64);
			};

			constexpr Boolean IsZero(SizeT Position) {
				if (Position > BitCount)
					Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
				return !BitCheck(bitMap[Position / 8], Position % 8);
			};

			constexpr void ResetAll() {
				Memory::MemBlkFill(bitMap, size, 0);
			};

			constexpr SizeT SettedBitCount() {
				SizeT Count = 0;
				for (SizeT byte = 0; byte < fullBytesCount; byte++) {
					if (bitMap[byte] == 0)
						Count += 8;
					else
						for (SizeT j = 0; j < 8; j++)
							if (BitCheck(bitMap[byte], j))
								Count++;
				}
				if constexpr (fullBytesCount > size) {
					for (SizeT j = 0; j < 8; j++)
						if (BitCheck(bitMap[fullBytesCount], j))
							Count++;
				}
				return Count;
			};

			constexpr BitMapT & operator&=(const BitMapT & Second) {
				for (SizeT i = 0; i < fullBytesCount; i++)
				{
					this->bitMap[i] &= Second.bitMap[i];
				}

				for (SizeT i = 0; i < LastByteBitsCount; i++)
				{
					if (Seed::BitCheck(this->bitMap[fullBytesCount], i) && Seed::BitCheck(Second.bitMap[fullBytesCount], i))
						Seed::BitSet(this->bitMap[fullBytesCount], i);
					else
						Seed::BitReset(this->bitMap[fullBytesCount], i);
				}
			}

			constexpr BitMapT & operator|=(const BitMapT & Second) {
				for (SizeT i = 0; i < fullBytesCount; i++)
				{
					this->bitMap[i] |= Second.bitMap[i];
				}

				for (SizeT i = 0; i < LastByteBitsCount; i++)
				{
					if (Seed::BitCheck(this->bitMap[fullBytesCount], i) || Seed::BitCheck(Second.bitMap[fullBytesCount], i))
						Seed::BitSet(this->bitMap[fullBytesCount], i);
					else
						Seed::BitReset(this->bitMap[fullBytesCount], i);
				}
			}

			constexpr bool operator==(BitMapT & Second) {
				for (SizeT i = 0; i < fullBytesCount; i++)
				{
					if (this->bitMap[i] != Second.bitMap[i])
						return false;
				}

				for (SizeT i = 0; i < LastByteBitsCount; i++)
				{
					if (Seed::BitCheck(this->bitMap[fullBytesCount], i) == Seed::BitCheck(Second.bitMap[fullBytesCount], i))
						return false;
				}
				return true;
			}

			constexpr bool operator!=(BitMapT & Second) {
				for (SizeT i = 0; i < fullBytesCount; i++)
				{
					if (this->bitMap[i] != Second.bitMap[i])
						return true;
				}

				for (SizeT i = 0; i < LastByteBitsCount; i++)
				{
					if (Seed::BitCheck(this->bitMap[fullBytesCount], i) == Seed::BitCheck(Second.bitMap[fullBytesCount], i))
						return true;
				}
				return false;
			}
		};
	}
}