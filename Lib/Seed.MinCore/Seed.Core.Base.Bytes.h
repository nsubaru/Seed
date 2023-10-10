#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.TypeTraits.h"

namespace Seed::Core::Internals {
	extern "C" unsigned __int64 __cdecl _byteswap_uint64(unsigned __int64);
	extern "C" unsigned long __cdecl _byteswap_ulong(unsigned long);
	extern "C" unsigned short __cdecl _byteswap_ushort(unsigned short);
}

namespace Seed::Core
{
	/// <summary>
	/// General functions for working with bytes ordering
	/// </summary>
	class Bytes
	{
	public:
		/// <summary>
		/// Reverses the bytes order for Int32
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		static Int32 ReverseByteOrder(Int32 value)
		{
			return Internals::_byteswap_ulong(value);
		}

		static UInt32 ReverseByteOrder(UInt32 value)
		{
			return Internals::_byteswap_ulong(value);
		}

		static Int16 ReverseByteOrder(Int16 value)
		{
			return Internals::_byteswap_ushort(value);
		}

		static UInt16 ReverseByteOrder(UInt16 value)
		{
			return Internals::_byteswap_ushort(value);
		}

		static Int64 ReverseByteOrder(Int64 value)
		{
			return Internals::_byteswap_uint64(value);
		}

		static UInt64 ReverseByteOrder(UInt64 value)
		{
			return Internals::_byteswap_uint64(value);
		}

		template<typename T> 
		requires TypeTraits::HasTriviaCtor<T>
		static T ReverseByteOrder(T value)
		{
			union {
				T data;
				Byte arr[sizeof(T)];
			};

			data = value;

			for (SizeT i = 0; i < sizeof(T) / 2; i++)
			{
				Swap(arr[i], arr[sizeof(T) - (i + 1)]);
			}

			return data;
		}
	};
}