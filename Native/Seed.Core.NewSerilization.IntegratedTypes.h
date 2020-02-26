#pragma once
#include "Seed.Core.NewSerilization.Base.h"

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
                template<typename T>
                constexpr bool IsSerilizableEnum = TypeTraits::IsEnumV<T>;

                template<typename THeap, typename T>
                struct BaseSerlizationHandler<THeap, T, typename TypeTraits::EnableIfV<IsSerilizableEnum<T>, void>> {
                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        Memory::Copy(reinterpret_cast<const Byte*>(&data), rawStream + offset, sizeof(T)); offset += sizeof(T);
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        return sizeof(T);
                    }

                    forceinline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("Enum")());
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
                            BaseSerlizationHandler<THeap, typename TypeTraits::EnumUnderlyingTypeV<T>>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename THeap>
                struct BaseSerlizationHandler<THeap, DateTime, void> {
                    using T = DateTime;
                    using TValue = typename NewCTTI::TypeTraits::CTTITypeMetaData<T>::NumericType;

                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        auto value = data.TimeFromStartEpoch;
                        Memory::Copy(reinterpret_cast<const Byte*>(&value), rawStream + offset, sizeof(TValue)); offset += sizeof(TValue);
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        return sizeof(TValue);
                    }

                    forceinline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("DateTime")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            NewCTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                        data = *(reinterpret_cast<const TValue*>(rawStream + offset)); offset += sizeof(T);
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<T>().Resize<NewCTTI::TypeName<T>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TValue)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, NewCTTI::TypeTraits::CTTITypeMetaData<T>::NumericType>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename THeap>
                struct BaseSerlizationHandler<THeap, Date, void> {
                    using T = Date;
                    using PlainTime = typename NewCTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime;
                    using TValue = typename NewCTTI::TypeTraits::CTTITypeMetaData<PlainTime>::NumericType;

                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        auto value = static_cast<PlainTime>(data).TimeFromStartEpoch;
                        Memory::Copy(reinterpret_cast<const Byte*>(&value), rawStream + offset, sizeof(TValue)); offset += sizeof(TValue);
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        return sizeof(TValue);
                    }

                    forceinline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("DateTime")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            NewCTTI::Details::ToCharStr<sizeof(TValue)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                        auto value = *(reinterpret_cast<const TValue*>(rawStream + offset)); offset += sizeof(T);
                        data = T(PlainTime(value));
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<T>().Resize<NewCTTI::TypeName<T>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TValue)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, NewCTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename THeap>
                struct BaseSerlizationHandler<THeap, DateSpan, void> {
                    using T = DateSpan;
                    using PlainTime = typename NewCTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime;
                    using TValue = typename NewCTTI::TypeTraits::CTTITypeMetaData<PlainTime>::NumericType;

                    forceinline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset) {
                        auto value = static_cast<PlainTime>(data).TimeFromStartEpoch;
                        Memory::Copy(reinterpret_cast<const Byte*>(&value), rawStream + offset, sizeof(TValue)); offset += sizeof(TValue);
                    }

                    forceinline static constexpr SizeT DataSize(const T& data) {
                        return sizeof(TValue);
                    }

                    forceinline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    forceinline static constexpr auto TypeHint() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("DateTime")());
                    }

                    forceinline static constexpr auto ConstexprSize() {
                        return
                            NewCTTI::Details::ToCharStr<sizeof(TValue)>();
                    }

                    forceinline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset) {
                        auto value = *(reinterpret_cast<const TValue*>(rawStream + offset)); offset += sizeof(T);
                        data = T(PlainTime(value));
                    }

                    forceinline static constexpr auto TypeMetaData() {
                        return
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            NewCTTI::TypeName<T>().Resize<NewCTTI::TypeName<T>().Count - 1>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<static_cast<SizeT>(NewCTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            NewCTTI::Details::ToCharStr<sizeof(TValue)>() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, NewCTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime>::TypeMetaData() +
                            NewCTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };
            }
        }
    }
}

#pragma warning(pop)