#pragma once
#include "Seed.Core.Serilization.Struct.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
            namespace Details
            {
                template<typename T>
                constexpr bool IsSerilizableBox = 
                    CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == CTTI::TypeTraits::EType::Box
                    &&
                    CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;

                template<typename THeap, template<typename, typename> typename TLazy, typename T, typename TLazyHeap>
                struct BaseSerlizationHandler<THeap, TLazy<TLazyHeap, T>, typename TypeTraits::EnableIfV<IsSerilizableBox<TLazy<TLazyHeap, T>>, void>> {
                    ForceInline static void SerilizeImpl(const TLazy<TLazyHeap, T>& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        BaseSerlizationHandler<THeap, T>::SerilizeImpl(*data, rawStream, offset, registry);
                    }

                    ForceInline static constexpr SizeT DataSize(const TLazy<TLazyHeap, T>& data, UnusedData const TypesRegistry& registry) {
                        return BaseSerlizationHandler<THeap, T>::DataSize(*data, registry);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(TLazy<TLazyHeap, T>)>();
                    }

                    ForceInline static void DeserelizeImpl(TLazy<TLazyHeap, T>& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        T val = T();

                        BaseSerlizationHandler<THeap, T>::DeserelizeImpl(val, rawStream, offset, registry);

                        data = TLazy<TLazyHeap, T>(MoveRValue(val));
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TLazy<TLazyHeap, T>>().Resize<CTTI::TypeName<TLazy<TLazyHeap, T>>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TLazy<TLazyHeap, T>>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TLazy<TLazyHeap, T>)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };

                template<typename THeap, template<typename> typename TSmartPtr, typename T>
                struct BaseSerlizationHandler<THeap, TSmartPtr<T>, typename TypeTraits::EnableIfV<IsSerilizableBox<TSmartPtr<T>>, void>> {
                    ForceInline static void SerilizeImpl(const TSmartPtr<T>& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        BaseSerlizationHandler<THeap, T>::SerilizeImpl(*data, rawStream, offset, registry);
                    }

                    ForceInline static constexpr SizeT DataSize(const TSmartPtr<T>& data, UnusedData const TypesRegistry& registry) {
                        return BaseSerlizationHandler<THeap, T>::DataSize(*data, registry);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("")());
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(TSmartPtr<T>)>();
                    }

                    ForceInline static void DeserelizeImpl(TSmartPtr<T>& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        T* val = new T();

                        BaseSerlizationHandler<THeap, T>::DeserelizeImpl(*val, rawStream, offset, registry);

                        data = TSmartPtr<T>(MoveRValue(val));
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("{")()) +
                            CTTI::TypeName<TSmartPtr<T>>().Resize<CTTI::TypeName<TSmartPtr<T>>().Count - 1>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<TSmartPtr<T>>::TypeClass)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            CTTI::Details::ToCharStr<sizeof(TSmartPtr<T>)>() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            ConstexprSize() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            TypeHint() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("|")()) +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::TypeStringToArrayWithoutTrailZero(GenNGTypeString("}")());
                    }
                };
            }
        }
    }
}