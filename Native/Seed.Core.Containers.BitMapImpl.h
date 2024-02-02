#pragma once
#include "Seed.Core.Containers.BitMap.h"

namespace Seed::Core::Containers
{
	template<typename THeap, SizeT BitCount, typename T>
	ForceInline constexpr SizeT BaseBitMapT<THeap, BitCount, T>::GetCount()
	{
		return BitCount;
	}

	template<typename THeap, SizeT BitCount, typename T>
	constexpr BaseBitMapT<THeap, BitCount, T>::BaseBitMapT(Boolean IsSetAll)
	{
		if (IsSetAll)
		{
			for (SizeT i = 0; i < FullClustersCount; i++)
			{
				bitMap[i] = Math::Max<T>();
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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr SizeT BaseBitMapT<THeap, BitCount, T>::FindBitSequalFrom(SizeT StartPosition, SizeT RequireCount)
	{
		if (RequireCount == 0)
			return 0;

		if (StartPosition == 0)
		{
			return FindBitSequel(RequireCount);
		}
		else
		{
			SizeT RangeSize = 0;
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

			if constexpr (LastClusterBitsCount != 0)
			{
				for (SizeT bit = 0; bit < LastClusterBitsCount; bit++)
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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr SizeT BaseBitMapT<THeap, BitCount, T>::FindBitSequel(SizeT RequireCount)
	{
		if (RequireCount == 0)
			return 0;

		SizeT RangeSize = 0;
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
							return ((byte * BitCountInFragment) + bit) - (RequireCount - 1);
					}
				else
					if (IsFound == true)
						IsFound = false;
			}
		}

		if constexpr (LastClusterBitsCount != 0)
		{
			for (SizeT bit = 0; bit < LastClusterBitsCount; bit++)
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
							return ((FullClustersCount * BitCountInFragment) + bit) - (RequireCount - 1);
					}
				else
					if (IsFound == true)
						IsFound = false;
			}
		}
		return -1;
	};

	template<typename THeap, SizeT BitCount, typename T>
	constexpr void BaseBitMapT<THeap, BitCount, T>::SetBitSequel(SizeT Position, SizeT BitsCount)
	{
		if (Position + BitsCount > BitCount)
			Throw(IndexOutOfRange(SourcePosition, Position, BitCount + BitsCount));

		SizeT StartCluster = Position / BitCountInFragment;
		SizeT StartBit = Position % BitCountInFragment;

		SizeT EndCluster = (Position + BitsCount) / BitCountInFragment;
		SizeT EndBit = (Position + BitsCount) % BitCountInFragment;

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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr void BaseBitMapT<THeap, BitCount, T>::ResetBitSequel(SizeT Position, SizeT BitsCount)
	{
		if (Position + BitsCount > BitCount)
			Throw(IndexOutOfRange(SourcePosition, Position, BitCount + BitsCount));

		SizeT StartCluster = Position / BitCountInFragment;
		SizeT StartBit = Position % BitCountInFragment;

		SizeT EndCluster = (Position + BitsCount) / BitCountInFragment;
		SizeT EndBit = (Position + BitsCount) % BitCountInFragment;

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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr void BaseBitMapT<THeap, BitCount, T>::SetBit(SizeT Position)
	{
		if (Position > BitCount)
			Throw(IndexOutOfRange(SourcePosition, Position, BitCount));
		Bits::BitSet(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
	};

	template<typename THeap, SizeT BitCount, typename T>
	constexpr void BaseBitMapT<THeap, BitCount, T>::ResetBit(SizeT Position)
	{
		if (Position > BitCount)
			Throw(IndexOutOfRange(SourcePosition, Position, BitCount));
		Bits::BitReset(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
	};

	template<typename THeap, SizeT BitCount, typename T>
	constexpr Boolean BaseBitMapT<THeap, BitCount, T>::IsZero(SizeT Position)
	{
		if (Position > BitCount)
			Throw(IndexOutOfRange(SourcePosition, Position, BitCount));
		return !Bits::BitCheck(bitMap[Position / BitCountInFragment], Position % BitCountInFragment);
	};

	template<typename THeap, SizeT BitCount, typename T>
	ForceInline constexpr void BaseBitMapT<THeap, BitCount, T>::ResetAll()
	{
		Memory::MemBlkFill(bitMap, Size, 0);
	};

	template<typename THeap, SizeT BitCount, typename T>
	constexpr SizeT BaseBitMapT<THeap, BitCount, T>::SetBitCount()
	{
		SizeT setCount = 0;
		for (SizeT byte = 0; byte < FullClustersCount; byte++)
		{
			if (bitMap[byte] == 0)
				setCount += BitCountInFragment;
			else
				for (SizeT j = 0; j < BitCountInFragment; j++)
					if (Bits::BitCheck(bitMap[byte], j))
						setCount++;
		}
		if constexpr (FullClustersCount > Size)
		{
			for (SizeT j = 0; j < BitCountInFragment; j++)
				if (Bits::BitCheck(bitMap[FullClustersCount], j))
					setCount++;
		}
		return setCount;
	};

	template<typename THeap, SizeT BitCount, typename T>
	constexpr BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::operator&=(const BaseBitMapT& Second)
	{
		for (SizeT i = 0; i < FullClustersCount; i++)
		{
			this->bitMap[i] &= Second.bitMap[i];
		}

		for (SizeT i = 0; i < LastClusterBitsCount; i++)
		{
			if (Bits::BitCheck(this->bitMap[FullClustersCount], i) && Bits::BitCheck(Second.bitMap[FullClustersCount], i))
				Bits::BitSet(this->bitMap[FullClustersCount], i);
			else
				Bits::BitReset(this->bitMap[FullClustersCount], i);
		}
	}

	template<typename THeap, SizeT BitCount, typename T>
	constexpr BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::operator|=(const BaseBitMapT& Second)
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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr bool BaseBitMapT<THeap, BitCount, T>::operator==(BaseBitMapT& Second)
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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr bool BaseBitMapT<THeap, BitCount, T>::operator!=(BaseBitMapT& Second)
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

	template<typename THeap, SizeT BitCount, typename T>
	constexpr T& BaseBitMapT<THeap, BitCount, T>::operator[](SizeT Index)
	{
		if (Index < Size)
			return this->bitMap[Index];
		else
			Throw(IndexOutOfRange(SourcePosition, Index, Size));
	}

	template<typename THeap, SizeT BitCount, typename T>
	constexpr const T& BaseBitMapT<THeap, BitCount, T>::operator[](SizeT Index) const
	{
		if (Index < Size)
			return this->bitMap[Index];
		else
			Throw(IndexOutOfRange(SourcePosition, Index, Size));
	}

	template<typename THeap, SizeT BitCount, typename T>
	BaseBitMapT<THeap, BitCount, T>::operator const typename BaseBitMapT<THeap, BitCount, T>::BitMapDataType() const&
	{
		return (const BitMapDataType) bitMap;
	}

	template<typename THeap, SizeT BitCount, typename T>
	BaseBitMapT<THeap, BitCount, T>::operator typename BaseBitMapT<THeap, BitCount, T>::BitMapDataType()&
	{
		return bitMap;
	}

#pragma warning(push)
#pragma warning(disable: 4172)

	template<typename THeap, SizeT BitCount, typename T>
	template<typename LambdaT>
	BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::ForEachImpl(LambdaT&& lambda)
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
			static_assert(TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&> ||
				TypeTraits::IsLambdaConvertibleV<LambdaT, void, T&, SizeT>);
		}
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename LambdaT>
	const BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::ForEachImpl(LambdaT&& lambda) const
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
			static_assert(TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&> ||
				TypeTraits::IsLambdaConvertibleV<LambdaT, void, const T&, SizeT>);
		}
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename U, typename LambdaT>
	U BaseBitMapT<THeap, BitCount, T>::AggregateImpl(LambdaT&& lambda) const
	{
		U val = U();
		for (SizeT i = 0; i < Size; i++)
		{
			lambda(val, bitMap[i]);
		}
		return val;
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename U, typename LambdaT>
	T BaseBitMapT<THeap, BitCount, T>::AggregateImpl(LambdaT&& lambda, const U& Value) const
	{
		U result = Value;
		for (SizeT i = 0; i < Size; i++)
		{
			lambda(result, bitMap[i]);
		}
		return MoveRValue(result);
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename U, typename LambdaT>
	U BaseBitMapT<THeap, BitCount, T>::AggregateImpl(LambdaT&& lambda, U&& Value) const
	{
		for (SizeT i = 0; i < Size; i++)
		{
			lambda(Value, bitMap[i]);
		}
		return MoveRValue(Value);
	}

	template<typename THeap, SizeT BitCount, typename T>
	bool BaseBitMapT<THeap, BitCount, T>::AnyImpl() const
	{
		return Size > 0;
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	bool BaseBitMapT<THeap, BitCount, T>::AnyImpl(Lambda&& lambda) const
	{
		for (SizeT i = 0; i < Size; i++)
		{
			if (lambda(bitMap[i]))
				return true;
		}
		return false;
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	bool BaseBitMapT<THeap, BitCount, T>::AllImpl(Lambda&& lambda) const
	{
		for (SizeT i = 0; i < Size; i++)
		{
			if (!lambda(bitMap[i]))
				return false;
		}
		return true;
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	T& BaseBitMapT<THeap, BitCount, T>::FirstImpl(Lambda&& lambda)
	{
		for (SizeT i = 0; i < Size; i++)
			if (lambda(bitMap[i]))
				return static_cast<T&>(bitMap[i]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition);
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	T& BaseBitMapT<THeap, BitCount, T>::LastImpl(Lambda&& lambda)
	{
		SizeT Index = -1;
		for (SizeT i = 0; i < Size; i++)
			if (lambda(bitMap[i]))
				Index = i;

		if (Index != -1)
			return static_cast<T&>(bitMap[Index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition);
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	const T& BaseBitMapT<THeap, BitCount, T>::FirstImpl(Lambda&& lambda) const
	{
		for (SizeT i = 0; i < Size; i++)
			if (lambda(bitMap[i]))
				return static_cast<T&>(bitMap[i]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition);
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	const T& BaseBitMapT<THeap, BitCount, T>::LastImpl(Lambda&& lambda) const
	{
		SizeT Index = -1;
		for (SizeT i = 0; i < Size; i++)
			if (lambda(bitMap[i]))
				Index = i;

		if (Index != -1)
			return static_cast<T&>(bitMap[Index]);

		Internals::Exceptions::ThrowNullPointException(SourcePosition);
	}

	template<typename THeap, SizeT BitCount, typename T>
	ForceInline BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::ReverseImpl()
	{
		for (SizeT i = 0; i < Size / 2; i++)
		{
			Swap(bitMap[i], bitMap[(Size - 1) - i]);
		}
		return *this;
	}

	template<typename THeap, SizeT BitCount, typename T>
	ForceInline void BaseBitMapT<THeap, BitCount, T>::UnsafeSet(const T& Value, SizeT Index)
	{
		bitMap[Index] = Value;
	}

	template<typename THeap, SizeT BitCount, typename T>
	ForceInline void BaseBitMapT<THeap, BitCount, T>::UnsafeSet(T&& Value, SizeT Index)
	{
		bitMap[Index] = MoveRValue(Value);
	}

	template<typename THeap, SizeT BitCount, typename T>
	ForceInline BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::RemoveAllImpl()
	{
		this->ResetAll();
		return *this;
	}

	template<typename THeap, SizeT BitCount, typename T>
	template<typename Lambda>
	BaseBitMapT<THeap, BitCount, T>& BaseBitMapT<THeap, BitCount, T>::RemoveAllImpl(Lambda&& lambda)
	{
		for (SizeT i = 0; i < Size; i++)
		{
			lambda(MoveRValue(bitMap[i]));
			bitMap[i] = 0;
		}
		return *this;
	}

#pragma warning(pop)
}