#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.TypeTraits.h"
#include "Seed.Core.Base.Swap.h"

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
		constexpr static Int32 ReverseByteOrder(Int32 value)
		{
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				union {
					Int32 data;
					Byte arr[sizeof(Int32)];
				};

				data = value;

				for (SizeT i = 0; i < sizeof(Int32) / 2; i++)
				{
					Swap(arr[i], arr[sizeof(Int32) - (i + 1)]);
				}

				return data;
			}
			else 
			{
				return Internals::_byteswap_ulong(value);
			}
		}

		/// <summary>
		/// Reverses the bytes order for UInt32
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		constexpr static UInt32 ReverseByteOrder(UInt32 value)
		{
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				union {
					UInt32 data;
					Byte arr[sizeof(UInt32)];
				};

				data = value;

				for (SizeT i = 0; i < sizeof(UInt32) / 2; i++)
				{
					Swap(arr[i], arr[sizeof(UInt32) - (i + 1)]);
				}

				return data;
			}
			else
			{
				return Internals::_byteswap_ulong(value);
			}
		}

		/// <summary>
		/// Reverses the bytes order for Int16
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		constexpr static Int16 ReverseByteOrder(Int16 value)
		{
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				union {
					Int16 data;
					Byte arr[sizeof(Int16)];
				};

				data = value;

				for (SizeT i = 0; i < sizeof(Int16) / 2; i++)
				{
					Swap(arr[i], arr[sizeof(Int16) - (i + 1)]);
				}

				return data;
			}
			else
			{
				return Internals::_byteswap_ushort(value);
			}
		}

		/// <summary>
		/// Reverses the bytes order for UInt16
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		constexpr static UInt16 ReverseByteOrder(UInt16 value)
		{
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				union {
					UInt16 data;
					Byte arr[sizeof(UInt16)];
				};

				data = value;

				for (SizeT i = 0; i < sizeof(UInt16) / 2; i++)
				{
					Swap(arr[i], arr[sizeof(UInt16) - (i + 1)]);
				}

				return data;
			}
			else
			{
				return Internals::_byteswap_ushort(value);
			}
		}

		/// <summary>
		/// Reverses the bytes order for Int64
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		constexpr static Int64 ReverseByteOrder(Int64 value)
		{
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				union {
					Int64 data;
					Byte arr[sizeof(Int64)];
				};

				data = value;

				for (SizeT i = 0; i < sizeof(Int64) / 2; i++)
				{
					Swap(arr[i], arr[sizeof(Int64) - (i + 1)]);
				}

				return data;
			}
			else
			{
				return Internals::_byteswap_uint64(value);
			}
		}

		/// <summary>
		/// Reverses the bytes order for UInt64
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		constexpr static UInt64 ReverseByteOrder(UInt64 value)
		{
			if (TypeTraits::IsCompileTimeEvaluating())
			{
				union {
					UInt64 data;
					Byte arr[sizeof(UInt64)];
				};

				data = value;

				for (SizeT i = 0; i < sizeof(UInt64) / 2; i++)
				{
					Swap(arr[i], arr[sizeof(UInt64) - (i + 1)]);
				}

				return data;
			}
			else
			{
				return Internals::_byteswap_uint64(value);
			}
		}

		/// <summary>
		/// Reverses the bytes order for template type
		/// </summary>
		/// <param name="value">Given set of bytes</param>
		/// <returns>Reversed set of bytes</returns>
		template<typename T> 
		requires TypeTraits::HasTriviaCtor<T>
		constexpr static T ReverseByteOrder(T value)
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