#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.CTTI.h"

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
                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
                        __assume(false);
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        return sizeof(data);
                    }

                    forceinline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("N/A")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                         Throw(InvalidArgumentException(ExceptionPosition, L"Data", L"Invalid data type"));
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename T>
                constexpr bool IsSerilizablePrimitive = (
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Numeric ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Character ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::FloatNumeric ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Logic ||
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::POD
                    )
                    &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T,typename TypeTraits::EnableIfV<IsSerilizablePrimitive<T>, void>> {
                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, sizeof(T)); offset += sizeof(T);
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        return sizeof(data);
                    }

                    forceinline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                        data = *(reinterpret_cast<const T*>(rawStream + offset)); offset += sizeof(T);
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
							CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")()) +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename T>
                constexpr bool IsCFixedArraySerlizable = 
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::FixedArray &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsCFixedArraySerlizable<T>, void>> {
                    using ItemType = typename CTTI::TypeTraits::CTTITypeMetaData<T>::ItemType::Type;

                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = DataSize(data);
                            Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, dataSize); 
                            offset += dataSize;
                        }
                        else
                        {
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                BaseSerlizationHandler<THeap, ItemType>::SerilizeImpl(data[i], rawStream, offset);
                            }
                        }
                        
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
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

                    forceinline static constexpr Boolean IsMemCopy() {
                        return BaseSerlizationHandler<THeap, ItemType>::IsMemCopy();
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<CTTI::TypeTraits::CTTITypeMetaData<T>::Count>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = DataSize(data);
                            Memory::Copy(rawStream + offset, data, dataSize); offset += dataSize;
                        }
                        else
                        {
                            SizeT Size = 1;
                            for (SizeT i = 0; i < CTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                Size *= BaseSerlizationHandler<THeap, ItemType>::DeserelizeImpl(data[i], rawStream, offset);
                            }
                        }
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, ItemType>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };
            }

            template<typename THeap, typename T>
            forceinline Containers::BaseArray<THeap, Byte> BaseSerilize(const T& Object) {
                SizeT rawStreamSize = Details::BaseSerlizationHandler<THeap, T>::DataSize(Object);
                Byte* rawStream = new Byte[rawStreamSize];
                SizeT offset = 0;
                Details::BaseSerlizationHandler<THeap, T>::SerilizeImpl(Object, rawStream, offset);
                return Containers::BaseArray<THeap, Byte>(MoveRValue(rawStream), rawStreamSize);
            }

            template<typename THeap, typename T>
            forceinline void BaseDeserilize(const Containers::BaseArray<THeap, Byte>& stream, T& Object) {
                const Byte* rawStream = (const Byte*) stream;
                SizeT offset = 0;
                Details::BaseSerlizationHandler<THeap, T>::DeserelizeImpl(Object, rawStream, offset);
            }

            template<typename T>
            forceinline Containers::Array<Byte> Serilize(const T& Object) {
                return BaseSerilize<Memory::Heap, T>(Object);
            }

            template<typename T>
            forceinline Containers::SensitiveArray<Byte> SensitiveSerilize(const T& Object) {
                return BaseSerilize<Memory::SensitiveDataHeap, T>(Object);
            }

            template<typename T>
            forceinline void Deserilize(const Containers::Array<Byte> & stream, T& Object) {
                BaseDeserilize<Memory::Heap, T>(stream, Object);
            }

            template<typename T>
            forceinline T SensitiveDeserilize(const Containers::Array<Byte>& stream, T& Object) {
                BaseDeserilize<Memory::SensitiveDataHeap, T>(stream, Object);
            }

            template<typename THeap, typename T>
            forceinline BaseString<THeap> BaseMetaDataGen() {
                return BaseString<THeap>(Details::BaseSerlizationHandler<THeap, T>::TypeMetaData() + CTTI::Details::TypeStringToArray(GenNGTypeString("")()));
            }

            template<typename T>
            forceinline String MetaDataGen() {
                return String(Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData() + CTTI::Details::TypeStringToArray(GenNGTypeString("")()));
            }

            template<typename T>
            forceinline SensitiveString SensitiveMetaDataGen() {
                return SensitiveString(Details::BaseSerlizationHandler<Memory::SensitiveDataHeap, T>::TypeMetaData() + CTTI::Details::TypeStringToArray(GenNGTypeString("")()));
            }

            template<typename T>
            forceinline constexpr auto CTTIMetaDataGen() {
                return Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData() + CTTI::Details::TypeStringToArray(GenNGTypeString("")());
            }
        }
    }
}

#pragma warning(pop)