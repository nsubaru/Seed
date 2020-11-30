#pragma once
#include "Seed.Core.Serilization.Core.h"

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
                            Core::Details::CreateTypeStr(L"Enum");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            Core::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        data = *(reinterpret_cast<const T*>(rawStream + offset)); offset += sizeof(T);
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            Core::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<T>() +
                            Core::Details::CreateTypeStr(L"|") +
                            Core::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            Core::Details::CreateTypeStr(L"|") +
                            Core::Details::ToCharStr<sizeof(T)>() +
                            Core::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            Core::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            Core::Details::CreateTypeStr(L"|") +
                            BaseSerlizationHandler<THeap, typename TypeTraits::EnumUnderlyingTypeV<T>>::TypeMetaData() +
                            Core::Details::CreateTypeStr(L"}");
                    }

                    ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
                        return BaseTypeMetaDataRecord<THeap>(
                            CTTI::TypeName<T>(),
                            CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
                            sizeof(T),
                            sizeof(T),
                            TypeHint(),
                            { BaseSerlizationHandler<THeap, typename TypeTraits::EnumUnderlyingTypeV<T>>::GetTypeMetaDataRecord() }
                        );
                    }
                };
            }
        }
    }
}

#pragma warning(pop)