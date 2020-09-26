#pragma once
#include "Seed.Core.Serilization.Base.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
            namespace Details
            {
                template<typename T>
                constexpr bool IsSerilizableEnum = TypeTraits::IsEnumV<T>;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsSerilizableEnum<T>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, sizeof(T)); offset += sizeof(T);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return sizeof(T);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr(L"Enum");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        data = *(reinterpret_cast<const T*>(rawStream + offset)); offset += sizeof(T);
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, typename TypeTraits::EnumUnderlyingTypeV<T>>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr(L"}");
                    }
                };
            }
        }
    }
}

#pragma warning(pop)