#pragma once
#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Interlocked.h"
#include "Seed.Core.Base.Math.h"
#include "Seed.Core.Base.Comparing.h"
#include "Seed.Core.Base.BitOperations.h"
#include "Seed.Core.Base.CStr.h"
#include "Seed.Core.Base.Swap.h"
#include "Seed.Core.Base.TypeTraits.h"
#include "Seed.Core.Base.Concepts.h"
#include "Seed.Core.Base.IClonable.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4595)

namespace Seed {
    inline namespace Core {
        namespace Details {
            class CompileStringView {
            private:
                const Char* ptr;
                SizeT size;
            public:
                constexpr CompileStringView(const Char* ptr)
                    :ptr(ptr), size(CStr::Size(ptr))
                {}

                constexpr CompileStringView Offset(SizeT Size) {
                    size -= Size;
                    ptr += Size;
                    return *this;
                }

                constexpr CompileStringView& Shrink(SizeT Size) {
                    size -= Size;
                    return *this;
                }

                constexpr CompileStringView(const CompileStringView&) = default;
                constexpr CompileStringView(CompileStringView&&) = default;
                constexpr CompileStringView& operator=(const CompileStringView&) = default;
                constexpr CompileStringView& operator=(CompileStringView&&) = default;

                template<SizeT N>
                constexpr auto AsArray() {
                    ConstexprArray<Char, N> ret;
                    if constexpr (N == 0)
                    {
                        return ret;
                    }
                    else
                    {
                        for (SizeT i = 0; i < N; i++)
                            ret[i] = ptr[i];
                        return ret;
                    }
                }

                constexpr SizeT Find(Char ch) {
                    for (SizeT i = 0ui64; i < size; i++)
                        if (ptr[i] == ch)
                            return i;
                    return static_cast<SizeT>(-1);
                }

                constexpr SizeT Find(Char ch, SizeT StartPos) {
                    for (SizeT i = StartPos; i < size; i++)
                        if (ptr[i] == ch)
                            return i;
                    return static_cast<SizeT>(-1);
                }

                constexpr SizeT Size() { return size; }

                template<typename T, SizeT Size>
                friend class ConstexprArray;
            };

            template<typename T, SizeT Size>
            class ConstexprArray {
            private:
                T arr[Size];
            public:
                template<typename C1, typename ...C>
                ForceInline friend constexpr auto Fill(ConstexprArray This, C1 c1, C ... c) {
                    This.arr[sizeof...(C)] = c1;
                    return Fill(This, c...);
                }
                template<typename C1>
                ForceInline friend constexpr auto Fill(ConstexprArray This, C1 c1) {
                    This.arr[0] = c1;
                    return This;
                }
            public:
                ForceInline constexpr ConstexprArray() : arr() {}

                template<typename ... T>
                ForceInline constexpr ConstexprArray(T ... t) : arr{ t... } {
                }

                ForceInline constexpr T& operator[](SizeT Index) {
                    return arr[Index];
                }

                ForceInline constexpr const T& operator[](SizeT Index) const {
                    return arr[Index];
                }

                template<SizeT N>
                ForceInline constexpr ConstexprArray<T, Size + N> operator+(ConstexprArray<T, N> second) const {
                    if constexpr (N == 0)
                    {
                        return *this;
                    }
                    else
                    {
                        ConstexprArray<T, Size + N> ret;
                        for (SizeT i = 0; i < Size; i++)
                            ret[i] = arr[i];
                        for (SizeT i = 0; i < N; i++)
                            ret[Size + i] = second.arr[i];
                        return ret;
                    }
                }

                template<SizeT NewSize>
                ForceInline constexpr ConstexprArray<T, NewSize> Resize() const {
                    ConstexprArray<T, NewSize> ret;
                    constexpr SizeT CopySize = Min(Size, NewSize);
                    for (SizeT i = 0; i < CopySize; i++)
                        ret[i] = arr[i];
                    return ret;
                }

                template<SizeT NewSize>
                ForceInline constexpr ConstexprArray<T, NewSize> ResizeLeft() const {
                    if constexpr (NewSize >= Size)
                        static_assert(false);
                    ConstexprArray<T, NewSize> ret;
                    constexpr SizeT Delta = Size - NewSize;
                    for (SizeT i = 0; i < NewSize; i++)
                        ret[i] = arr[i + Delta];
                    return ret;
                }

                ForceInline constexpr const T* Raw() const { return arr; }

                static constexpr SizeT Count = Size;

                ForceInline constexpr ConstexprArray Reverse() {
                    if constexpr (Size < 2)
                        return *this;
                    else
                    {
                        SizeT j = Size - 1;
                        for (SizeT i = 0; i < Size / 2; i++, j--) {
                            T t = arr[i];
                            arr[i] = arr[j];
                            arr[j] = t;
                        }
                        return *this;
                    }
                }

                template<SizeT Size2>
                ForceInline constexpr bool IsStartWith(ConstexprArray<T, Size2> mask) {
                    if (Size < Size2)
                        return false;

                    for (SizeT i = 0; i < Size2; i++)
                    {
                        if (arr[i] != mask[i])
                            return false;
                    }

                    return true;
                }

                constexpr SizeT Find(T item) {
                    for (SizeT i = 0ui64; i < Size; i++)
                        if (arr[i] == item)
                            return i;
                    return static_cast<SizeT>(-1);
                }

                constexpr SizeT Find(T item, SizeT StartPos) {
                    for (SizeT i = StartPos; i < Size; i++)
                        if (arr[i] == item)
                            return i;
                    return static_cast<SizeT>(-1);
                }

                template<typename T2, SizeT N>
                friend class ConstexprArray;
            };

            template<typename T>
            class ConstexprArray<T, 0> {
            public:
                ForceInline constexpr ConstexprArray() {};
                ForceInline constexpr ConstexprArray(const ConstexprArray&) = default;

                template<SizeT N>
                ForceInline constexpr ConstexprArray<T, N> operator+(ConstexprArray<T, N> second) const {
                    return second;
                }

                template<SizeT NewSize>
                ForceInline constexpr ConstexprArray<T, NewSize> Resize() const {
                    ConstexprArray<T, NewSize> ret;
                    return ret;
                }

                template<SizeT NewSize>
                ForceInline constexpr ConstexprArray<T, NewSize> ResizeLeft() const {
                    if constexpr (NewSize >= 0)
                        static_assert(false);
                    ConstexprArray<T, NewSize> ret;
                    return ret;
                }


                static constexpr SizeT Count = 0;

                ForceInline constexpr ConstexprArray Reverse() {
                    return *this;
                }

                constexpr SizeT Find(T item) {
                    return static_cast<SizeT>(-1);
                }

                constexpr SizeT Find(T item, SizeT StartPos) {
                    return static_cast<SizeT>(-1);
                }

                template<typename T2, SizeT N>
                friend class ConstexprArray;
            };

            namespace Details {
                template <typename U>
                ForceInline static constexpr auto type_name()
                {
                    SizeT prefix = 0;
                    while (true) {
                        if (__FUNCSIG__[prefix] == '<')
                            break;
                        prefix++;
                    }
                    prefix++;
                    SizeT suffix = CStr::Size(__FUNCSIG__) - 1;
                    while (true) {
                        if (__FUNCSIG__[suffix] == '>')
                            break;
                        suffix--;
                    }
                    suffix = CStr::Size(__FUNCSIG__) - suffix;
                    return CompileStringView(L"" __FUNCSIG__).Shrink(suffix).Offset(prefix);
                }

                template <typename U>
                ForceInline static constexpr auto type_name_size()
                {
                    SizeT prefix = 0;
                    while (true) {
                        if (__FUNCSIG__[prefix] == '<')
                            break;
                        prefix++;
                    }
                    prefix++;
                    SizeT suffix = CStr::Size(__FUNCSIG__) - 1;
                    while (true) {
                        if (__FUNCSIG__[suffix] == '>')
                            break;
                        suffix--;
                    }
                    return suffix - prefix;
                }

            }

            template<typename T>
            ForceInline constexpr auto TypeName() {
                return (Details::type_name<T>().AsArray<Details::type_name_size<T>()>());
            }

            template<SizeT I>
            ForceInline constexpr auto ToCharStr() {
                constexpr SizeT N = []() {
                    if (I < 9)
                        return 1ui64;
                    SizeT i = I; SizeT Count = 0;
                    while (i > 0)
                    {
                        Count++; i /= 10;
                    }
                    return Count;
                }();
                ConstexprArray<Char, N> ret;
                SizeT i = I; SizeT Pos = 0;
                if constexpr (I == 0)
                {
                    ret[0] = L'0';
                    return ret.Reverse();
                }
                else
                {
                    while (i > 0)
                    {
                        ret[Pos] = L'0' + i % 10;
                        Pos++; i /= 10;
                    }
                    return ret.Reverse();
                }
            }

            template <Seed::SizeT N>
            using FakeStr = Seed::Core::Details::ConstexprArray<Char, N>;


            template <Seed::SizeT N, Seed::SizeT ... Is>
            ForceInline constexpr FakeStr<sizeof...(Is)> MFSh(Char const (&str)[N],
                Seed::TypeTraits::Indexator<Is...> const&)
            {
                return { str[Is]... };
            }

            template <Seed::SizeT N>
            ForceInline constexpr auto CreateTypeStr(Char const (&str)[N])
            {
                return MFSh(str, Seed::TypeTraits::Indexer<N - 1>::Type{}).Reverse();
            }

            constexpr SizeT TypeStringSize = 2048;
        }
    }
}

#pragma warning(pop)