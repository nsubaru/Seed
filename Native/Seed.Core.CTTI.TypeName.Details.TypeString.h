#pragma once
#include "Seed.Core.Base.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace Details {
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

				template<SizeT N = Seed::Core::Math::Max<Seed::SizeT>(), SizeT M = Seed::Core::Math::Max<Seed::SizeT>()>
				constexpr char GetSymByN(char const(&c)[M]) noexcept
				{
#pragma warning(push)
#pragma warning(disable: 6385)
					static_assert(M != Seed::Math::Max<SizeT>(), "Invalid text size passed");
					__assume(M != Seed::Math::Max<SizeT>());
					return c[N < M ? N : M - 1];
#pragma warning(push)
				}

#define __GenNGTypeStringHelp(text, n)\
	Seed::Core::CTTI::Details::GetSymByN<0 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<1 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<2 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<3 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<4 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<5 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<6 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<7 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<8 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<9 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<10 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<11 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<12 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<13 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<14 + n>(text), \
	Seed::Core::CTTI::Details::GetSymByN<15 + n>(text)

#define __GetNGTypeStringHelp2(text, n) \
	__GenNGTypeStringHelp(text, 0 + n), \
	__GenNGTypeStringHelp(text, 16 + n), \
	__GenNGTypeStringHelp(text, 32 + n), \
	__GenNGTypeStringHelp(text, 48 + n), \

#define __GetNGTypeStringHelp3(text, n) \
	__GetNGTypeStringHelp2(text, 0 + n) \
	__GetNGTypeStringHelp2(text, 64 + n) \
	__GetNGTypeStringHelp2(text, 128 + n) \
	__GetNGTypeStringHelp2(text, 192 + n) \

#define __GetNGTypeStringHelp4(text, n) \
	__GetNGTypeStringHelp3(text, 0 + n) \
	__GetNGTypeStringHelp3(text, 256 + n) \
	__GetNGTypeStringHelp3(text, 512 + n) \
	__GetNGTypeStringHelp3(text, 768 + n) \

#ifndef __INTELLISENSE__
#define GenNGTypeString(text) \
	Seed::Core::CTTI::Details::TypeString<\
	__GetNGTypeStringHelp4(text, 0)\
	Seed::Core::CTTI::Details::GetSymByN(text)>

#else
#define GenNGTypeString(text) \
	Seed::Core::CTTI::Details::TypeString<\
	__GetNGTypeStringHelp3(text, 0)\
	Seed::Core::CTTI::Details::GetSymByN(text)>

#endif
			}
		}
	}
}

#pragma warning(pop)