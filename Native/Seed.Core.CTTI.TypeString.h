#pragma once
#include "Seed.Core.Base.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			template<char... C>
			struct TypeString final {
			private:
				static constexpr char const   vals[sizeof...(C) + 1] = { C...,'\0' };
				static constexpr SizeT sval = sizeof...(C);
			public:
				static constexpr char const* Data() noexcept
				{
					return &vals[0];
				}

				static constexpr SizeT FullSize() noexcept
				{
					return sval;
				};

				static constexpr SizeT Size() noexcept {
					return CStr::Size(vals) + 1;
				}
			};

			template<int N, int M>
			constexpr char GetSymByN(char const(&c)[M]) noexcept
			{
				return c[N < M ? N : M - 1];
			}

#define __GenTypeStringHelp(text, n)\
	Seed::Core::CTTI::GetSymByN<0 + n>(text), \
	Seed::Core::CTTI::GetSymByN<1 + n>(text), \
	Seed::Core::CTTI::GetSymByN<2 + n>(text), \
	Seed::Core::CTTI::GetSymByN<3 + n>(text), \
	Seed::Core::CTTI::GetSymByN<4 + n>(text), \
	Seed::Core::CTTI::GetSymByN<5 + n>(text), \
	Seed::Core::CTTI::GetSymByN<6 + n>(text), \
	Seed::Core::CTTI::GetSymByN<7 + n>(text), \
	Seed::Core::CTTI::GetSymByN<8 + n>(text), \
	Seed::Core::CTTI::GetSymByN<9 + n>(text), \
	Seed::Core::CTTI::GetSymByN<10 + n>(text), \
	Seed::Core::CTTI::GetSymByN<11 + n>(text), \
	Seed::Core::CTTI::GetSymByN<12 + n>(text), \
	Seed::Core::CTTI::GetSymByN<13 + n>(text), \
	Seed::Core::CTTI::GetSymByN<14 + n>(text), \
	Seed::Core::CTTI::GetSymByN<15 + n>(text)

#define __GetTypeStringHelp2(text, n) \
	__GenTypeStringHelp(text, 0 + n), \
	__GenTypeStringHelp(text, 16 + n), \
	__GenTypeStringHelp(text, 32 + n), \
	__GenTypeStringHelp(text, 48 + n), \

#define __GetTypeStringHelp3(text, n) \
	__GetTypeStringHelp2(text, 0 + n) \
	__GetTypeStringHelp2(text, 64 + n) \
	__GetTypeStringHelp2(text, 128 + n) \
	__GetTypeStringHelp2(text, 192 + n) \

#define __GetTypeStringHelp4(text, n) \
	__GetTypeStringHelp2(text, 0 + n) \
	__GetTypeStringHelp2(text, 256 + n) \
	__GetTypeStringHelp2(text, 512 + n) \
	__GetTypeStringHelp2(text, 768 + n) \

#define GenTypeString(text) \
	Seed::Core::CTTI::TypeString<\
	__GetTypeStringHelp4(text, 0)\
	Seed::Core::CTTI::GetSymByN<1024>(text)>
		}
	}
}

#pragma warning(pop)