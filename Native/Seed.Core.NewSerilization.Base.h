#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.NewCTTI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed
{
    inline namespace Core
    {
        namespace NewSerilization
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
                    !NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsNotSerlizable<T>, void>> {
                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        static_assert(false, "Cannot serelize this type");
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
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("N/A")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            NewCTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                         static_assert(false, "Cannot deserelize this type");
                         __assume(false);
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<T>().Resize<NewCTTI::TypeName<T>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(T)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")()) +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename T>
                constexpr bool IsSerilizablePrimitive = (
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Numeric ||
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Character ||
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::FloatNumeric ||
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::Logic ||
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::POD
                    )
                    &&
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

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
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            NewCTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                        data = *(reinterpret_cast<const T*>(rawStream + offset)); offset += sizeof(T);
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<T>().Resize<NewCTTI::TypeName<T>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(T)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
							NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")()) +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename T>
                constexpr bool IsCFixedArraySerlizable = 
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == NewCTTI::TypeTraits::EType::FixedArray &&
                    NewCTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsCFixedArraySerlizable<T>, void>> {
                    using ItemType = typename NewCTTI::TypeTraits::CTTITypeMetaData<T>::ItemType::Type;

                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        if constexpr (IsMemCopy())
                        {
                            SizeT dataSize = DataSize(data);
                            Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, dataSize); 
                            offset += dataSize;
                        }
                        else
                        {
                            for (SizeT i = 0; i < NewCTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                BaseSerlizationHandler<THeap, ItemType>::SerilizeImpl(data[i], rawStream, offset);
                            }
                        }
                        
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        if constexpr (IsMemCopy())
                        {
                            return sizeof(ItemType) * NewCTTI::TypeTraits::CTTITypeMetaData<T>::Count;
                        }
                        else
                        {
                            SizeT Size = 1;
                            for (SizeT i = 0; i < NewCTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
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
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            NewCTTI::Details::ToCharStr<NewCTTI::TypeTraits::CTTITypeMetaData<T>::Count>();
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
                            for (SizeT i = 0; i < NewCTTI::TypeTraits::CTTITypeMetaData<T>::Count; i++)
                            {
                                Size *= BaseSerlizationHandler<THeap, ItemType>::DeserelizeImpl(data[i], rawStream, offset);
                            }
                        }
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<T>().Resize<NewCTTI::TypeName<T>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(T)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, ItemType>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
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
                return BaseString<THeap>(Details::BaseSerlizationHandler<THeap, T>::TypeMetaData() + NewCTTI::Details::TypeStringToArray(GenNGTypeString("")()));
            }

            template<typename T>
            forceinline String MetaDataGen() {
                return String(Details::BaseSerlizationHandler<Memory::Heap, T>::TypeMetaData() + NewCTTI::Details::TypeStringToArray(GenNGTypeString("")()));
            }

            template<typename T>
            forceinline SensitiveString SensitiveMetaDataGen() {
                return SensitiveString(Details::BaseSerlizationHandler<Memory::SensitiveDataHeap, T>::TypeMetaData() + NewCTTI::Details::TypeStringToArray(GenNGTypeString("")()));
            }
        }
    }
}

#pragma warning(pop)