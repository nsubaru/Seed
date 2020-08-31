#pragma once
#include "typeinfo"
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.Containers.API.LINQ.IString.h"
#include "Seed.Core.Math.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.DateTime.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.NonConcurency.List.h"
#include "Seed.Core.Containers.NonConcurency.Map.h"
#include "Seed.Core.CTTI.TypeName.Details.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)

namespace Seed
{
	inline namespace Core
	{
		namespace Details
		{
			template<typename THeap, template<typename THeap> typename TString>
			class TLinqApi :
				public Containers::LINQ::IString<TString,THeap,UChar,false>
			{
            private:
				using TBaseString = TString<THeap>;
			protected:
                template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
                friend class Containers::LINQ::IStringCollectionBaseLinq;
                template<template<typename THeap> typename TString, typename THeap, typename TChar, Boolean TIsConcurencySuport>
                friend class Containers::LINQ::IString;
            protected:
                template<typename LambdaT>
                ForceInline TBaseString& ForEachImpl(LambdaT&& lambda) {
                    if constexpr (TypeTraits::IsInvockable<LambdaT, UChar&>) {
                        TBaseString* This = static_cast<TBaseString*>(this);
                        for (SizeT i = 0; i < This->Lenght; i++)
                        {
                            lambda(This->str[i]);
                        }
                        return *This;
                    }
                    else if constexpr (TypeTraits::IsInvockable<LambdaT, UChar&, SizeT>) {
                        TBaseString* This = static_cast<TBaseString*>(this);
                        for (SizeT i = 0; i < This->Lenght; i++)
                        {
                            lambda(This->str[i], i);
                        }
                        return *This;
                    }
                    else {
	                    static_assert(false);
                    }
                }

                template<typename LambdaT>
                ForceInline const TBaseString& ForEachImpl(LambdaT&& lambda) const {
                    if constexpr (TypeTraits::IsInvockable<LambdaT, const UChar&>) {
                        const TBaseString* This = static_cast<const TBaseString*>(this);
                        for (SizeT i = 0; i < This->Lenght; i++)
                        {
                            lambda(This->str[i]);
                        }
                        return *This;
                    }
                    else if constexpr (TypeTraits::IsInvockable<LambdaT, const UChar&, SizeT>) {
                        const TBaseString* This = static_cast<const TBaseString*>(this);
                        for (SizeT i = 0; i < This->Lenght; i++)
                        {
                            lambda(This->str[i], i);
                        }
                        return *This;
                    }
                    else {
	                    static_assert(false);
                    }
                }

                template<typename LambdaT>
                ForceInline TBaseString& ForEachImpl(LambdaT&& lambda, SizeT count) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    if (count > This->Count)
                        count = This->Count;
                    if constexpr (TypeTraits::IsInvockable<LambdaT, UChar&>) {
                        for (SizeT i = 0; i < count; i++)
                        {
                            lambda(This->str[i]);
                        }
                        return *This;
                    }
                    else if constexpr (TypeTraits::IsInvockable<LambdaT, UChar&, SizeT>) {
                        for (SizeT i = 0; i < count; i++)
                        {
                            lambda(This->str[i], i);
                        }
                        return *This;
                    }
                    else {
	                    static_assert(false);
                    }
                }

                template<typename LambdaT>
                ForceInline const TBaseString& ForEachImpl(LambdaT&& lambda, SizeT count) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);
                    if (count > This->Lenght)
                        count = This->Lenght;
                    if constexpr (TypeTraits::IsInvockable<LambdaT, const UChar&>) {
                        for (SizeT i = 0; i < count; i++)
                        {
                            lambda(This->str[i]);
                        }
                        return *This;
                    }
                    else if constexpr (TypeTraits::IsInvockable<LambdaT, const UChar&, SizeT>) {
                        for (SizeT i = 0; i < count; i++)
                        {
                            lambda(This->str[i], i);
                        }
                        return *This;
                    }
                    else {
	                    static_assert(false);
                    }
                }
            protected:
                bool AnyImpl() const { return this->Lenght > 0; }

                template<typename Lambda>
                ForceInline bool AnyImpl(Lambda&& lambda) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);
                    for (SizeT i = 0; i < This->Lenght; i++)
                    {
                        if (lambda(This->str[i]))
                            return true;
                    }
                    return false;
                }

                template<typename Lambda>
                ForceInline bool AllImpl(Lambda&& lambda) const {
                    const TBaseString* This = static_cast<const TBaseString*>(this);
                    for (SizeT i = 0; i < This->Lenght; i++)
                    {
                        if (!lambda(This->str[i]))
                            return false;
                    }
                    return true;
                }

                TBaseString& ReverseImpl() {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    for (SizeT i = 0; i < This->Lenght / 2; i++) {
                        Swap(This->str[i], This->str[(This->Lenght - 1) - i]);
                    }
                    return *This;
                }

                ForceInline void UnsafeSet(const UChar& Value, SizeT Index) {
                    static_cast<TBaseString*>(this)->str[Index] = Value;
                }

                ForceInline TBaseString& RemoveAllImpl() {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    This->~TBaseString();
                    return *This;
                }

                template<typename Lambda>
                ForceInline TBaseString& RemoveAllImpl(Lambda&& lambda) {
                    TBaseString* This = static_cast<TBaseString*>(this);
                    for (SizeT i = 0; i < This->CurrentSize; i++) {
                        lambda(MoveRValue(This->str[i]));
                    }
                    This->~TBaseString();
            	    return *This;
                }
            protected:
                virtual ~TLinqApi() = 0 {}
			};
		}
	}
}

#pragma warning(pop)