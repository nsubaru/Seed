#pragma once
#include "Seed.Core.Serilization.Base.h"
#include "Seed.Core.Serilization.TypeMetaRecord.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed
{
    inline namespace Core
    {
        namespace Reflection
        {
            class DataTypeRecord;
        }

        namespace RTTI
        {
            class RTTITypeRecord;
        }

        namespace Serilization
        {
            using TypesRegistry = Containers::ConcurencyList<Containers::Pair<RTTI::RTTITypeRecord, Reflection::DataTypeRecord>>;

            namespace Details
            {
#if __has_include("Seed.Core.Containers.API.LINQ.Base.h")
#else
                template<typename THeap, typename T, typename Enable = void>
                struct BaseSerlizationHandler;
#endif
                template<typename T>
                constexpr bool IsNotSerlizable = 
                    !CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsNotSerlizable<T>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                        __assume(false);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                        __assume(false);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                        __assume(false);
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            Core::Details::CreateTypeStr(L"N/A");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            Core::Details::ToCharStr<0>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                         Throw(NonSerelizableType(ExceptionPosition, CTTI::TypeName<T>()));
                         __assume(false);
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
                            Core::Details::CreateTypeStr(L"}");
                    }

                    ForceInline static TypeMetaDataRecord GetTypeMetaDataRecord() {
                        return TypeMetaDataRecord(
                            CTTI::TypeName<T>(), 
                            CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass, 
                            sizeof(T), 
                            0, 
                            TypeHint()
                        );
                    }
                };

                template<typename T>
                constexpr bool IsSerilizablePrimitive =
                    CTTI::IsSimpleTypes(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass) &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T,typename TypeTraits::EnableIfV<IsSerilizablePrimitive<T>, void>> {
                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, sizeof(T)); offset += sizeof(T);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return sizeof(data);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        if constexpr (CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::Character)
                            return CTTI::TypeTraits::CTTITypeMetaData<T>::CodePage;
                        else
                            return Core::Details::CreateTypeStr(L"");
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
                            Core::Details::CreateTypeStr(L"") +
                            Core::Details::CreateTypeStr(L"}");
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

                template<typename T>
                constexpr bool IsCFixedArraySerlizable = 
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::EType::FixedArray &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsCFixedArraySerlizable<T>, void>> {
                    using ItemType = typename CTTI::TypeTraits::CTTITypeMetaData<T>::ItemType::Type;

                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = DataSize(data, registry);
                            Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, dataSize); 
                            offset += dataSize;
                        }
                        else
                        {
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                BaseSerlizationHandler<THeap, ItemType>::SerilizeImpl(data[i], rawStream, offset, registry);
                            }
                        }
                        
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        if constexpr (IsMemCopy())
                        {
                            return sizeof(ItemType) * CTTI::TypeTraits::CTTITypeMetaData<T>::Count;
                        }
                        else
                        {
                            SizeT Size = 1;
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                Size *= BaseSerlizationHandler<THeap, ItemType>::DataSize(data[i]);
                            }
                            return Size;
                        }
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return BaseSerlizationHandler<THeap, ItemType>::IsMemCopy();
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            Core::Details::CreateTypeStr(L"");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            Core::Details::ToCharStr<CTTI::TypeTraits::CTTITypeMetaData<T>::Count>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = sizeof(T);
                            Memory::Copy(rawStream + offset, data, dataSize); offset += dataSize;
                        }
                        else
                        {
                            SizeT Size = 1;
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                Size *= BaseSerlizationHandler<THeap, ItemType>::DeserelizeImpl(data[i], rawStream, offset, registry);
                            }
                        }
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
                            BaseSerlizationHandler<THeap, ItemType>::TypeMetaData() +
                            Core::Details::CreateTypeStr(L"}");
                    }

                    ForceInline static BaseTypeMetaDataRecord<THeap> GetTypeMetaDataRecord() {
                        return BaseTypeMetaDataRecord<THeap>(
                            CTTI::TypeName<T>(), 
                            CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass, 
                            sizeof(T), 
                            CTTI::TypeTraits::CTTITypeMetaData<T>::Count, 
                            TypeHint(), 
                            { BaseSerlizationHandler<THeap, ItemType>::GetTypeMetaDataRecord() }
                        );
                    }
                };
            }
        }
    }
}

#pragma warning(pop)