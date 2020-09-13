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

                template<typename THeap, typename T, typename TLazyHeap>
                struct BaseSerlizationHandler<THeap, BaseLazy<TLazyHeap, T>, typename TypeTraits::EnableIfV<IsSerilizableBox<BaseLazy<TLazyHeap, T>>, void>> {
                    ForceInline static void SerilizeImpl(const BaseLazy<TLazyHeap, T>& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        BaseSerlizationHandler<THeap, T>::SerilizeImpl(*data, rawStream, offset, registry);
                    }

                    ForceInline static constexpr SizeT DataSize(const BaseLazy<TLazyHeap, T>& data, UnusedData const TypesRegistry& registry) {
                        return BaseSerlizationHandler<THeap, T>::DataSize(*data, registry);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(BaseLazy<TLazyHeap, T>)>();
                    }

                    ForceInline static void DeserelizeImpl(BaseLazy<TLazyHeap, T>& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        T val = T();

                        BaseSerlizationHandler<THeap, T>::DeserelizeImpl(val, rawStream, offset, registry);

                        data = BaseLazy<TLazyHeap, T>(MoveRValue(val));
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<BaseLazy<TLazyHeap, T>>().Resize<CTTI::TypeName<BaseLazy<TLazyHeap, T>>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<BaseLazy<TLazyHeap, T>>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(BaseLazy<TLazyHeap, T>)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };

                template<typename THeap, typename T, typename TSmartPtrHeap>
                struct BaseSerlizationHandler<THeap, BaseSmartPtr<TSmartPtrHeap, T>, typename TypeTraits::EnableIfV<IsSerilizableBox<BaseSmartPtr<TSmartPtrHeap, T>>, void>> {
                    ForceInline static void SerilizeImpl(const BaseSmartPtr<TSmartPtrHeap, T>& data, Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        BaseSerlizationHandler<THeap, T>::SerilizeImpl(*data, rawStream, offset, registry);
                    }

                    ForceInline static constexpr SizeT DataSize(const BaseSmartPtr<TSmartPtrHeap, T>& data, UnusedData const TypesRegistry& registry) {
                        return BaseSerlizationHandler<THeap, T>::DataSize(*data, registry);
                    }

                    ForceInline static constexpr Boolean IsMemCopy() {
                        return false;
                    }

                    ForceInline static constexpr auto TypeHint() {
                        return
                            CTTI::Details::CreateTypeStr("");
                    }

                    ForceInline static constexpr auto ConstexprSize() {
                        return
                            CTTI::Details::ToCharStr<sizeof(BaseSmartPtr<TSmartPtrHeap, T>)>();
                    }

                    ForceInline static void DeserelizeImpl(BaseSmartPtr<TSmartPtrHeap, T>& data, const Byte* rawStream, SizeT& offset, UnusedData const TypesRegistry& registry) {
                        T* val = new T();

                        BaseSerlizationHandler<THeap, T>::DeserelizeImpl(*val, rawStream, offset, registry);

                        data = BaseSmartPtr<TSmartPtrHeap, T>(MoveRValue(val));
                    }

                    ForceInline static constexpr auto TypeMetaData() {
                        return
                            CTTI::Details::CreateTypeStr("{") +
                            CTTI::TypeName<BaseSmartPtr<TSmartPtrHeap, T>>().Resize<CTTI::TypeName<BaseSmartPtr<TSmartPtrHeap, T>>().Count - 1>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<static_cast<SizeT>(CTTI::TypeTraits::CTTITypeMetaData<BaseSmartPtr<TSmartPtrHeap, T>>::TypeClass)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            CTTI::Details::ToCharStr<sizeof(BaseSmartPtr<TSmartPtrHeap, T>)>() +
                            CTTI::Details::CreateTypeStr("|") +
                            ConstexprSize() +
                            CTTI::Details::CreateTypeStr("|") +
                            TypeHint() +
                            CTTI::Details::CreateTypeStr("|") +
                            BaseSerlizationHandler<THeap, T>::TypeMetaData() +
                            CTTI::Details::CreateTypeStr("}");
                    }
                };
            }
        }
    }
}