#pragma once
#include "Seed.Core.Serilization.Core.h"

namespace Seed
{
    inline namespace Core
    {

        namespace Serilization
        {
            namespace Details
            {
                template<typename T>
                constexpr bool IsSerilizableMetData =
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::MetaData;

                template<typename THeap, typename TMetaDataHeap>
                struct BaseSerlizationHandler<THeap, BaseTypeMetaDataRecord<TMetaDataHeap>, typename TypeTraits::EnableIfV<IsSerilizableMetData<BaseTypeMetaDataRecord<TMetaDataHeap>>, void>> {
                    using T = typename BaseTypeMetaDataRecord<TMetaDataHeap>;
                    
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        BaseSerlizationHandler<THeap, typename CTTI::TypeTraits::CTTITypeMetaData<T>::StringType>::SerilizeImpl
                            (data.ToString(), rawStream, offset, registry);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return BaseSerlizationHandler<THeap, typename CTTI::TypeTraits::CTTITypeMetaData<T>::StringType>::DataSize
                            (data.ToString(), registry);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return CTTI::Details::CreateTypeStr(L"");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        typename CTTI::TypeTraits::CTTITypeMetaData<T>::StringType strValue = {};

                        BaseSerlizationHandler<THeap, typename CTTI::TypeTraits::CTTITypeMetaData<T>::StringType>::DeserelizeImpl
                            (strValue, rawStream, offset, registry);

                        data = BaseTypeMetaDataRecord<TMetaDataHeap>::DecodeTypeMetaDataRecord(strValue);
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr(L"{") +
                            CTTI::TypeName<T>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr(L"|") +
                            CTTI::Details::CreateTypeStr(L"") +
                            CTTI::Details::CreateTypeStr(L"}");
                    }

                    ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
                        return BaseTypeMetaDataRecord<THeap>(
                            CTTI::TypeName<T>(),
                            CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass,
                            sizeof(T),
                            sizeof(T),
                            TypeHint()
                            );
                    }
                };
            }
        }
    }
}