#pragma once
#include "Seed.Core.Base.h"

#pragma warning(push)
//Disable dll-interface requiring as Tuple template class, all class code is in header file so we don't need import/export anythink
#pragma warning(disable: 4251)

namespace Seed
{
    inline namespace Core
    {
        namespace Containers
        {
            template<typename... TDatas>
            class Tuple final {
            protected:
                template<UInt64 TN, typename THead, typename... TTypes>
                struct GetByN {
					static_assert(TN <= sizeof...(TTypes));
                    using Type = typename GetByN<TN - 1, TTypes...>::Type;
                };

                template<typename THead, typename... TTypes>
                struct GetByN<0, THead, TTypes...> {
					static_assert(0 <= sizeof...(TTypes));
                    using Type = THead;
                };

                template<UInt64 TI, typename TData>
                struct Leaf {
                    static constexpr UInt64 N = TI;
                    TData data;

                    forceinline constexpr const TData& GetData() const {
                        return data;
                    }

                    forceinline constexpr TData& GetData() {
                        return data;
                    }

                    forceinline constexpr TData&& MoveData()
                    {
                        return Seed::MoveRValue(data);
                    }

                    forceinline constexpr Leaf() = default;
                    forceinline constexpr Leaf(const TData & data) : data(data) {}
                    forceinline constexpr Leaf(TData && data) : data(ForwardReference<TData>(data)) {}

                    forceinline constexpr Leaf(const Leaf& leaf) : Leaf(leaf.data) {}
                    forceinline constexpr Leaf(Leaf&& leaf) : Leaf(ForwardReference<TData>(leaf.data)) {}

					forceinline constexpr Leaf& operator=(const Leaf& leaf) {
						this->Leaf::~Leaf();
						this->Leaf::Leaf(leaf);
					}

					forceinline constexpr Leaf& operator=(Leaf&& leaf) {
						this->Leaf::~Leaf();
						this->Leaf::Leaf(MoveRValue(leaf));
					}
                };

                template<UInt64 TI, typename TData>
                struct Leaf<TI, const TData&> {
                    static constexpr UInt64 N = TI;
                    TData data;

                    forceinline constexpr const TData& GetData() const {
                        return data;
                    }

                    forceinline constexpr TData& GetData() {
                        return data;
                    }

                    forceinline constexpr TData&& MoveData()
                    {
                        return Seed::MoveRValue(data);
                    }

                    forceinline constexpr Leaf() = default;
                    forceinline constexpr Leaf(const TData & data) : data(data) {}
                    forceinline constexpr Leaf(TData && data) : data(ForwardReference<TData>(data)) {}

                    forceinline constexpr Leaf(const Leaf& leaf) : Leaf(leaf.data) {}
                    forceinline constexpr Leaf(Leaf&& leaf) : Leaf(ForwardReference<TData>(leaf.data)) {}

					forceinline constexpr Leaf& operator=(const Leaf& leaf) {
						this->Leaf::~Leaf();
						this->Leaf::Leaf(leaf);
					}

					forceinline constexpr Leaf& operator=(Leaf&& leaf) {
						this->Leaf::~Leaf();
						this->Leaf::Leaf(MoveRValue(leaf));
					}
                };

                template<UInt64 TI>
                struct Leaf<TI, void> {
                    static constexpr UInt64 N = TI;
                };

                template<typename TIdx, typename... TTypes>
                struct Leafes;

                template<SizeT ... TIdxN, typename... TTypes>
                struct Leafes<TypeTraits::Indexator<TIdxN...>, TTypes... > : Leaf<TIdxN, TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TTypes>>>... {
                    template<UInt64 TId>
                    using RetType = typename GetByN<TId, TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TTypes>>...>::Type;

                    forceinline constexpr Leafes() = default;
                    forceinline constexpr Leafes(const Leafes&) = default;
                    forceinline constexpr Leafes(Leafes&&) = default;

                    forceinline constexpr Leafes(const TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TTypes>>&... datas) : Leaf<TIdxN, TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TTypes>>>(datas)... {}
                    forceinline constexpr Leafes(TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TTypes>>&&... datas) : Leaf<TIdxN, TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TTypes>>>(ForwardReference<TTypes>(datas))... {}

                    template<UInt64 TId>
                    forceinline constexpr decltype(auto) GetById() const {
                        using LeafType = Leaf<(Count - 1) - TId, RetType<TId>>;
                        decltype(auto) leaf = static_cast<const LeafType&>(*this);
                        decltype(auto) i = leaf.GetData();
                        return i;
                    }

                    template<UInt64 TId>
                    forceinline constexpr decltype(auto)  GetById() {
                        using LeafType = Leaf<(Count - 1) - TId, RetType<TId>>;
                        decltype(auto) leaf = static_cast<LeafType&>(*this);
                        decltype(auto) i = leaf.GetData();
                        return i;
                    }

                    template<UInt64 TId>
                    forceinline constexpr decltype(auto)  MoveByIdx() {
                        using LeafType = Leaf<(Count - 1) - TId, RetType<TId>>;
                        decltype(auto) leaf = static_cast<LeafType&>(*this);
                        decltype(auto) i = Seed::MoveRValue(leaf.MoveData());
                        return Seed::MoveRValue(i);
                    }

                    static constexpr SizeT Count = sizeof...(TTypes);
                };
            private:
                Leafes<typename TypeTraits::Indexer<sizeof...(TDatas)>::Type, TDatas...> ls;
            private:
                template<typename F, SizeT ... TIdxN>
                forceinline decltype(auto) CallWithArgsFromTupleImpl(F f, TypeTraits::TypeTag<TypeTraits::Indexator<TIdxN...>>) {
                    if constexpr (TypeTraits::IsSameV<decltype(f(MoveRValue(this->GetByIdx<(Count - 1) -TIdxN>())...)), void>)
                    {
                        f(MoveRValue<decltype(this->GetByIdx<(Count - 1) - TIdxN>())>(this->GetByIdx<(Count - 1) - TIdxN>())...);
                    }
                    else
                    {
                        return f(MoveRValue<decltype(this->GetByIdx<(Count - 1) - TIdxN>())>(this->GetByIdx<(Count - 1) - TIdxN>())...);
                    }
                }
            public:
                forceinline constexpr Tuple() : ls() {}
                forceinline constexpr Tuple(const Tuple&) = default;
                forceinline constexpr Tuple(Tuple&&) = default;

                forceinline constexpr Tuple(TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TDatas>>&&... data) : ls(ForwardReference<TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TDatas>>>(data)...) {}
                forceinline constexpr Tuple(const TypeTraits::RemoveRefV<TypeTraits::RemoveCV<TDatas>>&... data) : ls(data...) {}

                template<UInt64 TId>
                forceinline constexpr decltype(auto) GetByIdx() const {
                    return ForwardReference<decltype(ls.template GetById<TId>())>(ls.template GetById<TId>());
                }

                template<UInt64 TId>
                forceinline constexpr decltype(auto)  GetByIdx() {
                    return ForwardReference<decltype(ls.template GetById<TId>())>(ls.template GetById<TId>());
                }

                template<typename F>
                forceinline decltype(auto) CallWithArgsFromTuple(F f)
                {
                    if constexpr (TypeTraits::IsSameV<decltype(CallWithArgsFromTupleImpl(f, TypeTraits::TypeTag<typename TypeTraits::Indexer<sizeof...(TDatas)>::Type>{})), void>)
                    {
                        CallWithArgsFromTupleImpl(f, TypeTraits::TypeTag<typename TypeTraits::Indexer<sizeof...(TDatas)>::Type>{});
                    }
                    else
                    {
                        return CallWithArgsFromTupleImpl(f, TypeTraits::TypeTag<typename TypeTraits::Indexer<sizeof...(TDatas)>::Type>{});
                    }
                }

                template<UInt64 TI>
                // ReSharper disable once CppRedundantTypenameKeyword
                using ItemType = typename decltype(ls)::template RetType<TI>;

                static constexpr SizeT Count = sizeof...(TDatas);
            };

            template<>
            class Tuple<> final {
            public:
                forceinline constexpr Tuple() {}
                forceinline constexpr Tuple(const Tuple&) = default;
                forceinline constexpr Tuple(Tuple&&) = default;

                static constexpr SizeT Count = 0;

                template<typename F>
                forceinline decltype(auto) CallWithArgsFromTuple(F f) {
                    if constexpr (TypeTraits::IsSameV<decltype(f()), void>){
                        f();
                    }
                    else
                    {
                        return f();
                    }
                }
            };
        }

        namespace TypeTraits
        {
            template<typename T>
            struct IsTupleT : LogicFalse {};

            template<typename ... Types>
            struct IsTupleT<Containers::Tuple<Types...>> : LogicTrue {};

            template<typename T>
            constexpr bool IsTupleV = IsTupleT<T>::State;
        }

        namespace Concepts
        {
            template<typename T>
            concept Tuple = TypeTraits::IsTupleV<T>;
        }
    }
}

// ReSharper disable once CppInconsistentNaming
namespace std
{
    using Seed::SizeT;
#pragma region std::get
    template <SizeT TI, class... TType>
    // ReSharper disable once CppInconsistentNaming
    forceinline constexpr decltype(auto) get(Seed::Core::Containers::Tuple<TType...>& tuple) {
        return Seed::ForwardReference<decltype(tuple.template GetByIdx<TI>())>(tuple.template GetByIdx<TI>());
    }

    template <SizeT TI, class... TType>
    // ReSharper disable once CppInconsistentNaming
    forceinline constexpr decltype(auto) get(const Seed::Core::Containers::Tuple<TType...>& tuple) {
        return Seed::ForwardReference<decltype(tuple.template GetByIdx<TI>())>(tuple.template GetByIdx<TI>());
    }

    template <SizeT TI, class... TType>
    // ReSharper disable once CppInconsistentNaming
    forceinline constexpr decltype(auto) get(Seed::Core::Containers::Tuple<TType...>&& tuple) {
        return Seed::ForwardReference<decltype(tuple.template GetByIdx<TI>())>(tuple.template GetByIdx<TI>());
    }

    template <SizeT TI, class... TType>
    // ReSharper disable CppInconsistentNaming
    forceinline constexpr decltype(auto) get(const Seed::Core::Containers::Tuple<TType...>&& tuple) {
        // ReSharper restore CppInconsistentNaming
        return Seed::ForwardReference<decltype(tuple.template GetByIdx<TI>())>(tuple.template GetByIdx<TI>());
    }

#pragma endregion
#pragma region std::tuple_element
    template <size_t _Index, class _Tuple>
    // ReSharper disable once CppInconsistentNaming
    struct tuple_element;

    template <size_t TI, typename ... TTypes>
    struct tuple_element<TI, Seed::Core::Containers::Tuple<TTypes...>> {
        using ItemType = typename Seed::Core::Containers::Tuple<TTypes...>::template ItemType<TI>;
        using RefType = Seed::TypeTraits::AddLvalueReferenceV<ItemType>;
        // ReSharper disable once CppInconsistentNaming
        using type = decltype(get<TI>(Seed::Core::Containers::Tuple<TTypes...>()));
    };

    template <size_t TI, typename ... TTypes>
    struct tuple_element<TI, const Seed::Core::Containers::Tuple<TTypes...>> {
        using ItemType = typename Seed::Core::Containers::Tuple<TTypes...>::template ItemType<TI>;
        using ConstItemType = Seed::TypeTraits::AddConstV<ItemType>;
        using RefType = Seed::TypeTraits::AddLvalueReferenceV<ConstItemType>;
        // ReSharper disable once CppInconsistentNaming
        using type = RefType;
    };

    template <size_t TI, typename ... TTypes>
    struct tuple_element<TI, volatile Seed::Core::Containers::Tuple<TTypes...>> {
        using ItemType = typename Seed::Core::Containers::Tuple<TTypes...>::template ItemType<TI>;
        using VolatileItemType = Seed::TypeTraits::AddVolatileV<ItemType>;
        using RefType = Seed::TypeTraits::AddLvalueReferenceV<VolatileItemType>;
        // ReSharper disable once CppInconsistentNaming
        using type = RefType;
    };

    template <size_t TI, typename ... TTypes>
    struct tuple_element<TI, const volatile Seed::Core::Containers::Tuple<TTypes...>> {
        using ItemType = typename Seed::Core::Containers::Tuple<TTypes...>::template ItemType<TI>;
        using CVItemType = Seed::TypeTraits::AddCvV<ItemType>;
        using RefType = Seed::TypeTraits::AddLvalueReferenceV<CVItemType>;
        // ReSharper disable once CppInconsistentNaming
        using type = RefType;
    };
#pragma endregion
#pragma region std::tuple_size
    template<class T>
    // ReSharper disable once CppInconsistentNaming
    struct tuple_size;

    template<typename ... TTypes>
    struct tuple_size<Seed::Core::Containers::Tuple<TTypes...>> {
        // ReSharper disable once CppInconsistentNaming
        static constexpr SizeT value = sizeof...(TTypes);

        // ReSharper disable once CppNonExplicitConversionOperator
        forceinline constexpr operator SizeT() const {
            return value;
        }

        forceinline constexpr size_t operator()() const {
            return value;
        }

        // ReSharper disable once CppInconsistentNaming
        using value_type = size_t;
    };
#pragma endregion 
}

#pragma warning(pop)