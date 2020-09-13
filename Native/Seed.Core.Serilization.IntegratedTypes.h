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
                            CTTI::Details::CreateTypeStr("Enum");
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
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(T)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, typename TypeTraits::EnumUnderlyingTypeV<T>>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };

                template<typename THeap>
                struct BaseSerlizationHandler<THeap, DateTime, void> {
                    using T = DateTime;
                    using TValue = typename CTTI::TypeTraits::CTTITypeMetaData<T>::NumericType;

                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        auto value = data.TimeFromStartEpoch;
                        Memory::Copy(reinterpret_cast<const Byte*>(&value), rawStream + offset, sizeof(TValue)); offset += sizeof(TValue);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return sizeof(TValue);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("DateTime");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(T)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        data = *(reinterpret_cast<const TValue*>(rawStream + offset)); offset += sizeof(T);
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(TValue)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, CTTI::TypeTraits::CTTITypeMetaData<T>::NumericType>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };

                template<typename THeap>
                struct BaseSerlizationHandler<THeap, Date, void> {
                    using T = Date;
                    using PlainTime = typename CTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime;
                    using TValue = typename CTTI::TypeTraits::CTTITypeMetaData<PlainTime>::NumericType;

                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        auto value = static_cast<PlainTime>(data).TimeFromStartEpoch;
                        Memory::Copy(reinterpret_cast<const Byte*>(&value), rawStream + offset, sizeof(TValue)); offset += sizeof(TValue);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return sizeof(TValue);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("DateTime");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(TValue)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        auto value = *(reinterpret_cast<const TValue*>(rawStream + offset)); offset += sizeof(T);
                        data = T(PlainTime(value));
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(TValue)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, CTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };

                template<typename THeap>
                struct BaseSerlizationHandler<THeap, DateSpan, void> {
                    using T = DateSpan;
                    using PlainTime = typename CTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime;
                    using TValue = typename CTTI::TypeTraits::CTTITypeMetaData<PlainTime>::NumericType;

                    ForceInline static void SerilizeImpl(const T& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        auto value = static_cast<PlainTime>(data).TimeFromStartEpoch;
                        Memory::Copy(reinterpret_cast<const Byte*>(&value), rawStream + offset, sizeof(TValue)); offset += sizeof(TValue);
                    }

                    ForceInline static constexpr SizeT DataSize(const T& data, UnusedData const TypesRegistry& registry) {
                        return sizeof(TValue);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return true;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("DateTime");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(TValue)>();
                    }

                    ForceInline static void DeserelizeImpl(T& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        auto value = *(reinterpret_cast<const TValue*>(rawStream + offset)); offset += sizeof(T);
                        data = T(PlainTime(value));
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<T>().Resize<CTTI::TypeName<T>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(TValue)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, CTTI::TypeTraits::CTTITypeMetaData<T>::PlainTime>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };
            }
        }
    }
}

#pragma warning(pop)