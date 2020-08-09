#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Decay.h"
#include "Seed.Core.Base.TypeTraits.Utils.h"
#include "Seed.Core.Base.TypeTraits.LogicOperations.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
            template<class T>
            T* AddressOf(T& arg) noexcept
            {
                return reinterpret_cast<T*>(
                    &const_cast<char&>(
                        reinterpret_cast<const volatile char&>(arg)));
            }

            namespace Internals {
                template <class T> constexpr T& FUN(T& t) noexcept { return t; }
                template <class T> void FUN(T&&) = delete;
            }

            template<class T>
            class ReferenceWrapper {
            public:
                // types
                typedef T type;

                // construct/copy/destroy
                template <class U, class = decltype(
                    Internals::FUN<T>(DeclVal<U>()),
                    EnableIfV<!IsSameV<ReferenceWrapper, RemoveCVRefV<U>>, bool>()
                    )>
                    constexpr ReferenceWrapper(U&& u) noexcept(noexcept(Internals::FUN<T>(ForwardReference<U>(u))))
                    : _ptr(AddressOf(Internals::FUN<T>(ForwardReference<U>(u)))) {}
                ReferenceWrapper(const ReferenceWrapper&) noexcept = default;

                // assignment
                ReferenceWrapper& operator=(const ReferenceWrapper & x) noexcept = default;

                // access
                constexpr operator T& () const noexcept { return *_ptr; }
                constexpr T& get() const noexcept { return *_ptr; }

                template<class ...ArgTypes>
                constexpr auto
                    operator() (ArgTypes&&... args) const {
                    return decltype(auto);
                }


            private:
                T* _ptr;
            };

            template <class T>
            struct UnwrapRefWrapper
            {
                using Type = T;
            };

            template <class T>
            struct UnwrapRefWrapper<ReferenceWrapper<T>>
            {
                using Type = T&;
            };

            template <class T>
            using SpecialDecayT = typename UnwrapRefWrapper<typename Seed::Core::TypeTraits::DecayV<T>>::Type;
		}
	}
}