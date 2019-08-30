#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.String.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace CTTI {

			template<typename T, SizeT Size>
			class ConstexprArray;

			class CompileStringView {
			private:
				const AChar* ptr;
				SizeT size;
			public:
				constexpr CompileStringView(const AChar* ptr)
					:ptr(ptr), size(CStr::Size(ptr))
				{}

				template<SizeT N>
				constexpr CompileStringView(ConstexprArray<char, N> arr)
					: ptr(arr.arr), size(N)
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

				template<typename THeap>
				BaseString<THeap> toString() {
					return BaseString<THeap>(ptr, size + 1);
				}

				template<SizeT N>
				constexpr ConstexprArray<char, N> AsArray() {
					ConstexprArray<char, N> ret;
					for (SizeT i = 0; i < N; i++)
						ret[i] = ptr[i];
					return ret;
				}

				constexpr SizeT Find(char ch) {
					for (SizeT i = 0; i < size; i++)
						if (ptr[i] == ch)
							return i;
					return static_cast<SizeT>(-1);
				}

				constexpr SizeT Find(char ch, SizeT StartPos) {
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
				constexpr ConstexprArray() : arr() {}
				constexpr T& operator[](SizeT Index) {
					return arr[Index];
				}

				template<SizeT N>
				constexpr ConstexprArray<T, Size + N> operator+(ConstexprArray<T, N> second) {
					ConstexprArray<T, Size + N> ret;
					for (SizeT i = 0; i < Size; i++)
						ret[i] = arr[i];
					for (SizeT i = 0; i < N; i++)
						ret[Size + i] = second.arr[i];
					return ret;
				}

				template<SizeT NewSize>
				constexpr ConstexprArray<T, NewSize> Resize() {
					ConstexprArray<T, NewSize> ret;
					SizeT CopySize = Min(Size, NewSize);
					for (SizeT i = 0; i < CopySize; i++)
						ret[i] = arr[i];
					return ret;
				}
			};

			template<SizeT Size>
			class ConstexprString{
			private:
				const char(&text)[Size];
			public:
				constexpr ConstexprString(const char(&text)[Size]) :text(text) {}
			public:
				constexpr ConstexprArray<char, Size> AsArray() {
					ConstexprArray<char, Size> ret;
					for (SizeT i = 0; i < Size; i++)
						ret[i] = text[i];
					return ret;
				}
			};

			template<SizeT N>
			constexpr ConstexprString<N> MakeConstexprString(const char(&text)[N]) {
				return ConstexprString<N>(text);
			}



			constexpr CompileStringView OffsetStringView(CompileStringView text) {
				text.Offset(2);
				return text;
			}

			constexpr auto v = CompileStringView("test").Offset(2).Shrink(1);

			template <typename T>
			constexpr auto type_name()
			{
				constexpr auto prefix = CStr::Size("auto Seed::Core::CTTI::type_name(");
				constexpr auto suffix = CStr::Size(">()");
				return CompileStringView(__FUNCSIG__).Offset(prefix).Shrink(suffix);
			}

			template<typename T>
			constexpr auto TypeName() {
				return (type_name<int>().AsArray<type_name<int>().Size()>()).Resize<type_name<int>().Size() + 1>();
			}

			constexpr auto v2 = MakeConstexprString("text").AsArray().Resize<4>() + MakeConstexprString("text").AsArray();

			constexpr auto v3 = (type_name<int>().AsArray<type_name<int>().Size()>()).Resize<type_name<int>().Size() + 1>();

			constexpr auto v4 = TypeName<int>();
		}
	}
}

#pragma warning(pop)