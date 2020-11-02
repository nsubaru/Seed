#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.API.Linq.h"

namespace Seed {
	inline namespace Core
	{
		namespace Containers
		{
			/// <summary>
			/// Бітова карта незміного розміру, кількість бітів вказується аргументом BitCount
			/// BitCount не може бути більший ніж (2 * 1024 * 1024 * 1024 * 8) - 1, тобто 2Гб - 1 байт
			/// </summary>
			template<typename THeap, SizeT BitCount, typename T>
			class BaseBitMapT : 
				public Memory::Allocator<THeap>,
				public LINQ::IBitMapT<BaseBitMapT, THeap, T, Seed::Math::RoundToUpper(BitCount, sizeof(T) * 8ui64), true>
			{
			public:
				static constexpr SizeT BitCountInFragment = sizeof(T) * 8ui64;
			private:
				static_assert(BitCount != 0, "BitMap bit count must not equal zero");
				static_assert(Seed::Math::RoundToUpper(BitCount, BitCountInFragment) < (2Gb).Bytes, "BitMap Size must not bigger then 2GB - 1b");
			public:
				static constexpr SizeT Size = Seed::Math::RoundToUpper(BitCount, BitCountInFragment);
				static constexpr SizeT FullClustersCount = BitCount / BitCountInFragment;
				static constexpr SizeT LastClusterBitsCount = BitCount - (FullClustersCount * BitCountInFragment);
				
			public:
				/// <summary>
				/// Розмір карти в кількості бітів
				/// </summary>
				/// <returns>Кількість бітів</returns>
				ForceInline constexpr SizeT GetCount()
				{
					return BitCount;
				}

				/// <summary>
				/// Кількість біьів
				/// </summary>
				SizeT Property(GetCount) Count;
			private:
				T bitMap[Size] = { 0 };
			public:
				/// <summary>
				/// Конструктор, якщо виставлений флаг то відбудеться заповнення бітової карти
				/// </summary>
				/// <param name="IsSetAll">Якщо true то бітова карта створюється заповненою</param>
				ForceInline constexpr BaseBitMapT(Boolean IsSetAll = false)
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

				/// <summary>
				/// Пошук суцільної ділянки певного розміру
				/// </summary>
				/// <param name="StartPosition">Стартова позиція</param>
				/// <param name="RequireCount">Запрошуваний розмір ділянки</param>
				/// <returns>Номер першого елемента знайденої ділянки або -1 якщо не вдалось найти таку ділянку</returns>
				constexpr SizeT FindBitSequalFrom(SizeT StartPosition, SizeT RequireCount) {
					if (StartPosition == 0)
					{
						return FindBitSequal(RequireCount);
					}
					else
					{
						SizeT RangeSize;
						Boolean IsFound = false;

						SizeT bitInFirstCluster = StartPosition % BitCountInFragment;
						SizeT startCluster = StartPosition / BitCountInFragment;

						for (SizeT bit = bitInFirstCluster; bit < BitCountInFragment; bit++)
						{
							if (!Bits::BitCheck(bitMap[startCluster], bit))
								if (IsFound == false)
								{
									RangeSize = 0; IsFound = true;
								}
								else
								{
									RangeSize++;
									if (RangeSize == RequireCount)
										return ((StartPosition * BitCountInFragment) + bit) - RequireCount;
								}
							else
								if (IsFound == true)
									IsFound = false;
						}

						for (SizeT byte = startCluster + 1; byte < FullClustersCount; byte++)
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
						{
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
						}
						return -1;
					}
				}

				/// <summary>
				/// Пошук суцільної ділянки певного розміру
				/// </summary>
				/// <param name="RequireCount">Запрошуваний розмір ділянки</param>
				/// <returns>Номер першого елемента знайденої ділянки або -1 якщо не вдалось найти таку ділянку</returns>
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
					{
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
					}
					return -1;
				};

				/// <summary>
				/// Заповнення заданої ділянки
				/// </summary>
				/// <param name="Position">Стартовий індекс</param>
				/// <param name="BitsCount">Кількість елементів</param>
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
								bitMap[byte] = Math::Max<T>();
						}
					}
				};

				/// <summary>
				/// Звільнення заданої ділянки
				/// </summary>
				/// <param name="Position">Індекс першого елемента</param>
				/// <param name="BitsCount">Кількість бітів</param>
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

				/// <summary>
				/// Виставлення біта
				/// </summary>
				/// <param name="Position">Індекс елемента</param>
				constexpr void SetBit(SizeT Position)
				{
					if (Position > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
					Bits::BitSet(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
				};

				/// <summary>
				/// Скидання біта
				/// </summary>
				/// <param name="Position">Індекс елемента</param>
				constexpr void ResetBit(SizeT Position)
				{
					if (Position > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
					Bits::BitReset(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
				};

				/// <summary>
				/// Провірка чи за заданою позицією біт вільний
				/// </summary>
				/// <param name="Position">Індекс елемента</param>
				/// <returns>true якщо позиція вільна</returns>
				constexpr Boolean IsZero(SizeT Position)
				{
					if (Position > BitCount)
						Throw(IndexOutOfRange(ExceptionPosition, Position, BitCount));
					return !Bits::BitCheck(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
				};

				/// <summary>
				/// Скидання всіх бітів(операція може бути достатньо довгою)
				/// </summary>
				ForceInline constexpr void ResetAll()
				{
					Memory::MemBlkFill(bitMap, Size, 0);
				};

				/// <summary>
				/// Підрахунок кількості встановлених бітів
				/// </summary>
				/// <returns></returns>
				constexpr SizeT SettedBitCount()
				{
					SizeT settedCount = 0;
					for (SizeT byte = 0; byte < FullClustersCount; byte++)
					{
						if (bitMap[byte] == 0)
							settedCount += BitCountInFragment;
						else
							for (SizeT j = 0; j < BitCountInFragment; j++)
								if (Bits::BitCheck(bitMap[byte], j))
									settedCount++;
					}
					if constexpr (FullClustersCount > Size)
					{
						for (SizeT j = 0; j < BitCountInFragment; j++)
							if (Bits::BitCheck(bitMap[FullClustersCount], j))
								settedCount++;
					}
					return settedCount;
				};

				/// <summary>
				/// Об'єдання двох бітових карт через бітовe AND
				/// </summary>
				/// <param name="Second">Друга бітова карта</param>
				/// <returns>Поточна бітова карта після операції</returns>
				constexpr BaseBitMapT& operator&=(const BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						this->bitMap[i] &= Second.bitMap[i];
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Seed::Bits::BitCheck(this->bitMap[FullClustersCount], i) && Bits::BitCheck(Second.bitMap[FullClustersCount], i))
							Bits::BitSet(this->bitMap[FullClustersCount], i);
						else
							Bits::BitReset(this->bitMap[FullClustersCount], i);
					}
				}

				/// <summary>
				/// Об'єдання двох бітових карт через бітовe OR
				/// </summary>
				/// <param name="Second">Друга бітова карта</param>
				/// <returns>Поточна бітова карта після операції</returns>
				constexpr BaseBitMapT& operator|=(const BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						this->bitMap[i] |= Second.bitMap[i];
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Bits::BitCheck(this->bitMap[FullClustersCount], i) || Bits::BitCheck(Second.bitMap[FullClustersCount], i))
							Bits::BitSet(this->bitMap[FullClustersCount], i);
						else
							Bits::BitReset(this->bitMap[FullClustersCount], i);
					}
				}

				/// <summary>
				/// Оператор рівності двох бітових карт
				/// </summary>
				/// <param name="Second">Друга бітова карта</param>
				/// <returns>true якщо карти ідентичні</returns>
				constexpr bool operator==(BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						if (this->bitMap[i] != Second.bitMap[i])
							return false;
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Bits::BitCheck(this->bitMap[FullClustersCount], i) == Bits::BitCheck(Second.bitMap[FullClustersCount], i))
							return false;
					}
					return true;
				}

				/// <summary>
				/// Оператор не рівності двох бітових карт
				/// </summary>
				/// <param name="Second">Друга бітова карта</param>
				/// <returns>false якщо карти ідентичні</returns>
				constexpr bool operator!=(BaseBitMapT& Second)
				{
					for (SizeT i = 0; i < FullClustersCount; i++)
					{
						if (this->bitMap[i] != Second.bitMap[i])
							return true;
					}

					for (SizeT i = 0; i < LastClusterBitsCount; i++)
					{
						if (Bits::BitCheck(this->bitMap[FullClustersCount], i) == Bits::BitCheck(Second.bitMap[FullClustersCount], i))
							return true;
					}
					return false;
				}

				/// <summary>
				/// Оператор доступу до кластера бітів
				/// </summary>
				/// <param name="Index">Новер кластера</param>
				/// <returns>Силка на кластер</returns>
				constexpr T& operator[](SizeT Index)
				{
					if (Index < Size)
						return this->bitMap[Index];
					else
						Throw(IndexOutOfRange(ExceptionPosition, Index, Size));
				}

				/// <summary>
				/// Оператор доступу до кластера бітів
				/// </summary>
				/// <param name="Index">Новер кластера</param>
				/// <returns>Силка на кластер</returns>
				constexpr const T& operator[](SizeT Index) const
				{
					if (Index < Size)
						return this->bitMap[Index];
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
				ForceInline BaseBitMapT& ForEachImpl(LambdaT&& lambda)
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
				ForceInline const BaseBitMapT& ForEachImpl(LambdaT&& lambda) const
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
				ForceInline U AggregateImpl(LambdaT&& lambda) const
				{
					U val = U();
					for (SizeT i = 0; i < Size; i++)
					{
						lambda(val, bitMap[i]);
					}
					return val;
				}

				template<typename U, typename LambdaT>
				ForceInline T AggregateImpl(LambdaT&& lambda, const U& Value) const
				{
					U result = Value;
					for (SizeT i = 0; i < Size; i++)
					{
						lambda(result, bitMap[i]);
					}
					return MoveRValue(result);
				}

				template<typename U, typename LambdaT>
				ForceInline U AggregateImpl(LambdaT&& lambda, U&& Value) const
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
				ForceInline bool AnyImpl(Lambda&& lambda) const
				{
					for (SizeT i = 0; i < Size; i++)
					{
						if (lambda(bitMap[i]))
							return true;
					}
					return false;
				}

				template<typename Lambda>
				ForceInline bool AllImpl(Lambda&& lambda) const
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

				ForceInline BaseBitMapT& ReverseImpl()
				{
					for (SizeT i = 0; i < Size / 2; i++)
					{
						Swap(bitMap[i], bitMap[(Size - 1) - i]);
					}
					return *this;
				}

				ForceInline void UnsafeSet(const T& Value, SizeT Index)
				{
					bitMap[Index] = Value;
				}

				ForceInline void UnsafeSet(T&& Value, SizeT Index)
				{
					bitMap[Index] = MoveRValue(Value);
				}

				ForceInline BaseBitMapT& RemoveAllImpl()
				{
					this->ResetAll();
					return *this;
				}

				template<typename Lambda>
				ForceInline BaseBitMapT& RemoveAllImpl(Lambda&& lambda)
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

			/// <summary>
			/// Бітова карта незміного розміру, кількість бітів вказується аргументом BitCount
			/// BitCount не може бути більший ніж (2 * 1024 * 1024 * 1024 * 8) - 1, тобто 2Гб - 1 байт
			/// </summary>
			namespace
			{
				template<SizeT BitCount, typename T = Byte>
				using BitMapT = BaseBitMapT<Memory::Heap, BitCount, T>;
			}

			/// <summary>
			/// Бітова карта незміного розміру, кількість бітів вказується аргументом BitCount
			/// BitCount не може бути більший ніж (2 * 1024 * 1024 * 1024 * 8) - 1, тобто 2Гб - 1 байт
			/// </summary>
			namespace
			{
				template<SizeT BitCount, typename T = Byte>
				using SensitiveBitMapT = BaseBitMapT<Memory::SensitiveDataHeap, BitCount, T>;
			}
		}
	}
}