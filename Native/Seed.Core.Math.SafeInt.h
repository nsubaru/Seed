#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.ConstString.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
#pragma region SafeInt
			template<typename T>
			class SafeInt {
				//static_assert(false, "Safe int not implemented for this type");
			};

			template<>
			class SafeInt<UInt8>
			{
			private:
				UInt8 value;
			public:
				constexpr __forceinline SafeInt(UInt8 Value = 0) : value(Value) {};

#pragma region Simple operations
#pragma region SafeInt arg
				__forceinline SafeInt operator+(SafeInt second) {
					extern bool Add(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(SafeInt second) {
					extern bool Sub(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(SafeInt second) {
					extern bool Mul(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma region Int arg

				__forceinline SafeInt operator+(UInt8 second) {
					extern bool Add(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Add(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(UInt8 second) {
					extern bool Sub(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Sub(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(UInt8 second) {
					extern bool Mul(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Mul(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma endregion

#pragma region Extended operations
#pragma region SafeInt arg
				__forceinline SafeInt& operator+=(SafeInt second) {
					extern bool Add(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(SafeInt second) {
					extern bool Sub(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(SafeInt second) {
					extern bool Mul(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma region Int arg
				__forceinline SafeInt& operator+=(UInt8 second) {
					extern bool Add(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Add(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(UInt8 second) {
					extern bool Sub(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Sub(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(UInt8 second) {
					extern bool Mul(UInt8 First, UInt8 Second, UInt8 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt8 result;
					if (!Mul(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma endregion

#pragma region Get & Set

				constexpr __forceinline UInt8 Get() { return value; }
				constexpr __forceinline void Set(UInt8 Value) { this->value = Value; }

				UInt8 Property(Get, Set) Value;

#pragma endregion
			};

			template<>
			class SafeInt<UInt16>
			{
			private:
				UInt16 value;
			public:
				constexpr __forceinline SafeInt(UInt16 Value = 0) : value(Value) {};

#pragma region Simple operations
#pragma region SafeInt arg
				__forceinline SafeInt operator+(SafeInt second) {
					extern bool Add(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(SafeInt second) {
					extern bool Sub(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(SafeInt second) {
					extern bool Mul(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma region Int arg

				__forceinline SafeInt operator+(UInt16 second) {
					extern bool Add(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Add(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(UInt16 second) {
					extern bool Sub(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Sub(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(UInt16 second) {
					extern bool Mul(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Mul(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma endregion

#pragma region Extended operations
#pragma region SafeInt arg
				__forceinline SafeInt& operator+=(SafeInt second) {
					extern bool Add(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(SafeInt second) {
					extern bool Sub(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(SafeInt second) {
					extern bool Mul(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma region Int arg
				__forceinline SafeInt& operator+=(UInt16 second) {
					extern bool Add(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Add(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(UInt16 second) {
					extern bool Sub(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Sub(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(UInt16 second) {
					extern bool Mul(UInt16 First, UInt16 Second, UInt16 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt16 result;
					if (!Mul(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma endregion

#pragma region Get and Set

				constexpr __forceinline UInt16 Get() { return value; }
				constexpr __forceinline void Set(UInt16 Value) { this->value = Value; }

				UInt16 Property(Get, Set) Value;

#pragma endregion
			};

			template<>
			class SafeInt<UInt32>
			{
			private:
				UInt32 value;
			public:
				constexpr __forceinline SafeInt(UInt32 Value = 0) : value(Value) {};

#pragma region Simple operations
#pragma region SafeInt arg
				__forceinline SafeInt operator+(SafeInt second) {
					extern bool Add(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(SafeInt second) {
					extern bool Sub(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(SafeInt second) {
					extern bool Mul(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma region Int arg

				__forceinline SafeInt operator+(UInt32 second) {
					extern bool Add(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Add(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(UInt32 second) {
					extern bool Sub(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Sub(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(UInt32 second) {
					extern bool Mul(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Mul(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma endregion

#pragma region Extended operations
#pragma region SafeInt arg
				__forceinline SafeInt& operator+=(SafeInt second) {
					extern bool Add(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(SafeInt second) {
					extern bool Sub(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(SafeInt second) {
					extern bool Mul(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma region Int arg
				__forceinline SafeInt& operator+=(UInt32 second) {
					extern bool Add(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Add(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(UInt32 second) {
					extern bool Sub(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Sub(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(UInt32 second) {
					extern bool Mul(UInt32 First, UInt32 Second, UInt32 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt32 result;
					if (!Mul(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma endregion

#pragma region Get and Set

				constexpr __forceinline UInt32 Get() { return value; }
				constexpr __forceinline void Set(UInt32 Value) { this->value = Value; }

				UInt32 Property(Get, Set) Value;

#pragma endregion
			};

			template<>
			class SafeInt<UInt64>
			{
			private:
				UInt64 value;
			public:
				constexpr __forceinline SafeInt(UInt64 Value = 0) : value(Value) {};

#pragma region Simple operations
#pragma region SafeInt arg
				__forceinline SafeInt operator+(SafeInt second) {
					extern bool Add(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(SafeInt second) {
					extern bool Sub(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(SafeInt second) {
					extern bool Mul(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma region Int arg

				__forceinline SafeInt operator+(UInt64 second) {
					extern bool Add(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Add(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator-(UInt64 second) {
					extern bool Sub(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Sub(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}

				__forceinline SafeInt operator*(UInt64 second) {
					extern bool Mul(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Mul(this->value, second, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					return result;
				}
#pragma endregion
#pragma endregion

#pragma region Extended operations
#pragma region SafeInt arg
				__forceinline SafeInt& operator+=(SafeInt second) {
					extern bool Add(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Add(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(SafeInt second) {
					extern bool Sub(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Sub(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(SafeInt second) {
					extern bool Mul(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Mul(this->value, second.value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma region Int arg
				__forceinline SafeInt& operator+=(UInt64 second) {
					extern bool Add(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Add(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator-=(UInt64 second) {
					extern bool Sub(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Sub(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}

				__forceinline SafeInt& operator*=(UInt64 second) {
					extern bool Mul(UInt64 First, UInt64 Second, UInt64 & Result);
					extern __declspec(noreturn) void ThrowIntegerOverflowException(
						const Seed::ConstAString & File,
						const Seed::ConstAString & Function,
						const Seed::SizeT Line);
					UInt64 result;
					if (!Mul(this->value, value, result))
						ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
					this->value = result;
					return *this;
				}
#pragma endregion
#pragma endregion

#pragma region Get and Set

				constexpr __forceinline UInt64 Get() { return value; }
				constexpr __forceinline void Set(UInt64 Value) { this->value = Value; }

				UInt64 Property(Get, Set) Value;

#pragma endregion
			};
#pragma endregion

#pragma region ConvertUnsignedToSigned
			template<typename T>
			__forceinline StaticAsserts::Signed<T> ConvertUnsignedToSigned(StaticAsserts::Unsigned<T> From) {
				extern bool ConvertToSigned(StaticAsserts::Unsigned<T> Value, StaticAsserts::Signed<T> & Result);
				extern __declspec(noreturn) void ThrowIntegerOverflowException(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Line);
				StaticAsserts::Signed<T> ret;
				if (!ConvertToSigned(From, ret))
					ThrowIntegerOverflowException(__FILE__, __FUNCSIG__, __LINE__);
				return ret;
			}
#pragma endregion
		}
	}
}