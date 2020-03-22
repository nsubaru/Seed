#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.Linq.h"

namespace Seed {
	inline namespace Core
	{
		namespace Containers
		{
			//Бітова карта незміного розміру, кількість бітів вказується аргументом BitCount
			//BitCount не може бути більший ніж (2 * 1024 * 1024 * 1024 * 8) - 1, тобто 2Гб - 1 байт
			template<typename THeap, SizeT BitCount, typename T>
			class BaseBitMapT : 
				public Memory::Allocator<THeap>,
				public LINQ::IBitMapT<BaseBitMapT, THeap, T, Seed::Math::RoundToUpper(BitCount, sizeof(T) * 8ui64), true>
			{
			public:
				static constexpr SizeT BitCountInFragment = sizeof(T) * 8ui64;
			private:
				static_assert(BitCount != 0, "BitMap bit count must not equal zero");
				static_assert(Seed::Math::RoundToUpper(BitCount, BitCountInFragment) < 2Gb, "BitMap Size must not bigger then 2GB - 1b");
			public:
				static constexpr SizeT Size = Seed::Math::RoundToUpper(BitCount, BitCountInFragment);
				static constexpr SizeT FullClustersCount = BitCount / BitCountInFragment;
				static constexpr SizeT LastClusterBitsCount = BitCount - (FullClustersCount * BitCountInFragment);
				
			public:
				__forceinline constexpr SizeT GetCount()
				{
					return BitCount;
				}
				SizeT Property(GetCount) Count;
			private:
				T bitMap[Size] = { 0 };
			public:
				__forceinline constexpr BaseBitMapT(Boolean IsSetAll = false)
				{
					if (IsSetAll)
					{
						for (SizeT i = 0; i < FullClustersCount; i++)
						{
							bitMap[i] = Seed::Math::Max<T>();
						}
						if constexpr (FullClustersCount < Size)
						{
							for (SizeT i = 0; i < LastClusterBitsCount; i++)
							{
								Bits::BitSet<T>(bitMap[FullClustersCount], i);
							}
						}
					}
				}

				constexpr SizeT FindBitSequal(SizeT RequireCount)
				{
					SizeT RangeSize;
					Boolean IsFound = false;
					for (SizeT byte = 0; byte < FullClustersCount; byte++)
					{
						if (bitMap[byte] == Math::Max<T>())
							continue;
						for (SizeT bit = 0; bit < BitCountInFragment; bit++)
						{
							if (!Bits::BitCheck(bitMap[byte], bit))
								if (IsFound == false)
								{
									RangeSize = 0; IsFound = true;
								}
								else
								{
									RangeSize++;
									if (RangeSize == RequireCount)
										return ((byte * BitCountInFragment) + bit) - RequireCount;
								}
							else
								if (IsFound == true)
									IsFound = false;
						}
					}
					if constexpr (FullClustersCount < Size)
						for (SizeT bit = 0; bit < BitCountInFragment; bit++)
						{
							if (!Bits::BitCheck(bitMap[FullClustersCount], bit))
								if (IsFound == false)
								{
									RangeSize = 0; IsFound = true;
								}
								else
								{
									RangeSize++;
									if (RangeSize == RequireCount)
										return ((FullClustersCount * BitCountInFragment) + bit) - RequireCount;
								}
							else
								if (IsFound == true)
									IsFound = false;
						}
					return -1;
				};

				constexpr void SetBitSequal(SizeT Position, SizeT BitsCount)
				{
					if (Position + BitsCount > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount + BitsCount));

					SizeT StartCluster = Position / BitCountInFragment;
					SizeT StartBit = Position % BitCountInFragment;

					SizeT EndCluster = ((Position / BitCountInFragment) + BitsCount);
					SizeT EndBit = ((Position % BitCountInFragment) + BitsCount) % BitCountInFragment;

					EndBit++;

					if (StartCluster == EndCluster)
					{
						for (SizeT bit = StartBit; bit < EndBit; bit++)
							Bits::BitSet(bitMap[StartCluster], bit);
					}
					else
					{
						if (StartBit != 0)
						{
							for (SizeT bit = StartBit; bit < BitCountInFragment; bit++)
								Bits::BitSet(bitMap[StartCluster], bit);
						}
						else
						{
							bitMap[StartCluster] = Math::Max<T>();
						}
						if (EndBit != BitCountInFragment)
						{
							for (SizeT bit = 0; bit < EndBit; bit++)
								Bits::BitSet(bitMap[EndCluster], bit);
						}
						else
						{
							bitMap[EndCluster] = Math::Max<T>();
						}
						if (StartCluster - EndCluster > 1)
						{
							for (SizeT byte = StartCluster + 1; byte < EndCluster; byte++)
								bitMap[byte] = Math::Max::T;
						}
					}
				};
				constexpr void ResetBitSequal(SizeT Position, SizeT BitsCount)
				{
					if (Position + BitsCount > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount + BitsCount));

					SizeT StartCluster = Position / BitCountInFragment;
					SizeT StartBit = Position % BitCountInFragment;

					SizeT EndCluster = ((Position / BitCountInFragment) + BitsCount);
					SizeT EndBit = ((Position % BitCountInFragment) + BitsCount) % BitCountInFragment;

					EndBit++;

					if (StartCluster == EndCluster)
					{
						for (SizeT bit = StartBit; bit < EndBit; bit++)
							Bits::BitReset(bitMap[StartCluster], bit);
					}
					else
					{
						if (StartBit != 0)
						{
							for (SizeT bit = StartBit; bit < BitCountInFragment; bit++)
								Bits::BitReset(bitMap[StartCluster], bit);
						}
						else
						{
							bitMap[StartCluster] = 0;
						}
						if (EndBit != BitCountInFragment)
						{
							for (SizeT bit = 0; bit < EndBit; bit++)
								Bits::BitReset(bitMap[EndCluster], bit);
						}
						else
						{
							bitMap[EndCluster] = 0;
						}
						if (StartCluster - EndCluster > 1)
						{
							for (SizeT byte = StartCluster + 1; byte < EndCluster; byte++)
								bitMap[byte] = 0;
						}
					}
				};

				constexpr void SetBit(SizeT Position)
				{
					if (Position > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
					Bits::BitSet(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
				};

				constexpr void ResetBit(SizeT Position)
				{
					if (Position > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
					Bits::BitReset(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
				};

				constexpr Boolean IsZero(SizeT Position)
				{
					if (Position > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
					return !BitCheck(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
				};

				__forceinline constexpr void ResetAll()
				{
					Memory::MemBlkFill(bitMap, Size, 0);
				};

				constexpr SizeT SettedBitCount()
				{
					SizeT Count = 0;
					for (SizeT byte = 0; byte < FullClustersCount; byte++)
					{
						if (bitMap[byte] == 0)
							Count += BitCountInFragment;
						else
							for (SizeT j = 0; j < BitCountInFragment; j++)
								if (BitCheck(bitMap[byte], j))
									Count++;
					}
					if constexpr (FullClustersCount > Size)
					{
						for (SizeT j = 0; j < BitCountInFragment; j++)
							if (BitCheck(bitMap[FullClustersCount], j))
								Count++;
					}
					return Count;
				};

				constexpr BaseBitMapT& operator&=(const BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						this->bitMap[i] &= Second.bitMap[i];
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Seed::BitCheck(this->bitMap[FullClustersCount], i) && Seed::BitCheck(Second.bitMap[FullClustersCount], i))
							Seed::BitSet(this->bitMap[FullClustersCount], i);
						else
							Seed::BitReset(this->bitMap[FullClustersCount], i);
					}
				}

				constexpr BaseBitMapT& operator|=(const BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						this->bitMap[i] |= Second.bitMap[i];
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Seed::BitCheck(this->bitMap[FullClustersCount], i) || Seed::BitCheck(Second.bitMap[FullClustersCount], i))
							Bits::Seed::BitSet(this->bitMap[FullClustersCount], i);
						else
							Bits::Seed::BitReset(this->bitMap[FullClustersCount], i);
					}
				}

				constexpr bool operator==(BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						if (this->bitMap[i] != Second.bitMap[i])
							return false;
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Seed::BitCheck(this->bitMap[FullClustersCount], i) == Seed::BitCheck(Second.bitMap[FullClustersCount], i))
							return false;
					}
					return true;
				}

				constexpr bool operator!=(BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						if (this->bitMap[i] != Second.bitMap[i])
							return true;
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Seed::BitCheck(this->bitMap[FullClustersCount], i) == Seed::BitCheck(Second.bitMap[FullClustersCount], i))
							return true;
					}
					return false;
				}

				constexpr T& operator[](SizeT Index)
				{
					if (Index < Size)
						return this->bitMap[Index]
					else
						Throw(IndexOutOfRange(ExceptionPosition, Index, Size));
				}

				constexpr const T& operator[](SizeT Index) const
				{
					if (Index < Size)
						return this->bitMap[Index]
					else
						Throw(IndexOutOfRange(ExceptionPosition, Index, Size));
				}
			protected:
				template<template<typename, typename> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollection;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT Size, Boolean TIsConcurencySuport>
				friend class LINQ::IFixedCollection;
				template<template<typename, SizeT, typename> typename TCollection, typename THeap, typename T, SizeT Size, Boolean TIsConcurencySuport>
				friend class LINQ::IBitMapT;
				template<template<typename THeap, typename T> typename TCollection, typename THeap, typename T, Boolean TIsDynamic, Boolean TIsConcurencySuport>
				friend class LINQ::ICollectionBase;
                template<typename THeap, typename T, typename Enable>
                friend struct Seed::Core::Serilization::Details::BaseSerlizationHandler;
			protected:
#pragma warning(push)
#pragma warning(disable: 4172)

				template<typename LambdaT>
				__forceinline BaseBitMapT& ForEachImpl(LambdaT&& lambda)
				{
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&>)
					{
						for (SizeT i = 0; i < Size; i++)
						{
							lambda(bitMap[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>)
					{
						for (SizeT i = 0; i < Size; i++)
						{
							lambda(bitMap[i], i);
						}
						return *this;
					}
					else
					{
						static_assert(false);
					}
				}

				template<typename LambdaT>
				__forceinline const BaseBitMapT& ForEachImpl(LambdaT&& lambda) const
				{
					if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&>)
					{
						for (SizeT i = 0; i < Size; i++)
						{
							lambda(bitMap[i]);
						}
						return *this;
					}
					else if constexpr (TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>)
					{
						for (SizeT i = 0; i < Size; i++)
						{
							lambda(bitMap[i], i);
						}
						return *this;
					}
					else
					{
						static_assert(false);
					}
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda) const
				{
					U val = U();
					for (SizeT i = 0; i < Size; i++)
					{
						lambda(val, bitMap[i]);
					}
					return val;
				}

				template<typename U, typename LambdaT>
				__forceinline T AggregateImpl(LambdaT&& lambda, const U& Value) const
				{
					U result = Value;
					for (SizeT i = 0; i < Size; i++)
					{
						lambda(result, bitMap[i]);
					}
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				__forceinline U AggregateImpl(LambdaT&& lambda, U&& Value) const
				{
					for (SizeT i = 0; i < Size; i++)
					{
						lambda(Value, bitMap[i]);
					}
					return MoveRValue(Value);
				}

				bool AnyImpl() const
				{
					return Size > 0;
				}

				template<typename Lambda>
				__forceinline bool AnyImpl(Lambda&& lambda) const
				{
					for (SizeT i = 0; i < Size; i++)
					{
						if (lambda(bitMap[i]))
							return true;
					}
					return false;
				}

				template<typename Lambda>
				__forceinline bool AllImpl(Lambda&& lambda) const
				{
					for (SizeT i = 0; i < Size; i++)
					{
						if (!lambda(bitMap[i]))
							return false;
					}
					return true;
				}

				template<typename Lambda>
				T& FirstImpl(Lambda&& lambda)
				{
					for (SizeT i = 0; i < Size; i++)
						if (lambda(bitMap[i]))
							return static_cast<T&>(bitMap[i]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				T& LastImpl(Lambda&& lambda)
				{
					SizeT Index = -1;
					for (SizeT i = 0; i < Size; i++)
						if (lambda(bitMap[i]))
							Index = i;

					if (Index != -1)
						return static_cast<T&>(bitMap[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& FirstImpl(Lambda&& lambda) const
				{
					for (SizeT i = 0; i < Size; i++)
						if (lambda(bitMap[i]))
							return static_cast<T&>(bitMap[i]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				template<typename Lambda>
				const T& LastImpl(Lambda&& lambda) const
				{
					SizeT Index = -1;
					for (SizeT i = 0; i < Size; i++)
						if (lambda(bitMap[i]))
							Index = i;

					if (Index != -1)
						return static_cast<T&>(bitMap[Index]);

					Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCSIG__, __LINE__, 0, 0);
				}

				__forceinline BaseBitMapT& ReverseImpl()
				{
					for (SizeT i = 0; i < Size / 2; i++)
					{
						Swap(bitMap[i], bitMap[(Size - 1) - i]);
					}
					return *this;
				}

				__forceinline void UnsafeSet(const T& Value, SizeT Index)
				{
					bitMap[Index] = Value;
				}

				__forceinline void UnsafeSet(T&& Value, SizeT Index)
				{
					bitMap[Index] = MoveRValue(Value);
				}

				__forceinline BaseBitMapT& RemoveAllImpl()
				{
					this->ResetAll();
					return *this;
				}

				template<typename Lambda>
				__forceinline BaseBitMapT& RemoveAllImpl(Lambda&& lambda)
				{
					for (SizeT i = 0; i < Size; i++)
					{
						lambda(MoveRValue(bitMap[i]));
						bitMap[i] = 0;
					}
					return *this;
				}
#pragma warning(pop)

			};

			namespace
			{
				template<SizeT BitCount, typename T = Byte>
				using BitMapT = BaseBitMapT<Memory::Heap, BitCount, T>;
			}

			namespace
			{
				template<SizeT BitCount, typename T = Byte>
				using SensitiveBitMapT = BaseBitMapT<Memory::SensitiveDataHeap, BitCount, T>;
			}
		}
	}
}