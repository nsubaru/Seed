#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Math.Internal.h"
#include "Seed.Core.Math.SafeInt.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			inline constexpr Double Pi = Internal::Pi();
			inline constexpr Double E = Internal::E();
#pragma region Abs
			forceinline constexpr Int8 Abs(Int8 Val)
			{
				return Internal::ConstexprAbs(Val);
			}

			forceinline constexpr Int16 Abs(Int16 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			forceinline constexpr Int32 Abs(Int32 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			forceinline constexpr Int64 Abs(Int64 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			forceinline constexpr Float Abs(Float Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprAbs(Val)))
					return Internal::ConstexprAbs(Val);
				else
					return Internal::RuntimeAbs(Val);
			}

			forceinline constexpr Double Abs(Double Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprAbs(Val)))
					return Internal::ConstexprAbs(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
#pragma endregion
#pragma region Pow
			forceinline constexpr UInt64 Pow(UInt64 X, UInt8 Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
			forceinline constexpr Double Pow(Double X, Int32 Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
			forceinline constexpr Double Pow(Double X, Double Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
#pragma endregion
#pragma region Log
			forceinline constexpr Double Ln(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLn(X)))
					return Internal::ConstexprLn(X);
				else
					return Internal::RuntimeLn(X);
			}
			forceinline constexpr Double Lg(Double X)
			{
				return Ln(X);
			}
			forceinline constexpr Double Log2(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLog2(X)))
					return Internal::ConstexprLog2(X);
				else
					return Internal::RuntimeLog2(X);
			}
#pragma endregion
#pragma region Sqrt
			forceinline constexpr Double Sqrt(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSqrt(X)))
					return Internal::ConstexprSqrt(X);
				else
					return Internal::RuntimeSqrt(X);
			}
#pragma endregion

#pragma region Sin
			forceinline constexpr Double Sin(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSin(X)))
					return Internal::ConstexprSin(X);
				else
					return Internal::RuntimeSin(X);
			}
#pragma endregion

#pragma region Cos
			forceinline constexpr Double Cos(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprCos(X)))
					return Internal::ConstexprCos(X);
				else
					return Internal::RuntimeCos(X);
			}
#pragma endregion
#pragma region Tan
			forceinline constexpr Double Tan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprTan(X)))
					return Internal::ConstexprTan(X);
				else
					return Internal::RuntimeTan(X);
			}
#pragma endregion
#pragma region CTan
			forceinline constexpr Double CTan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprCtan(X)))
					return Internal::ConstexprCtan(X);
				else
					return Internal::RuntimeCtan(X);
			}
#pragma endregion
#pragma region ASin
			forceinline constexpr Double ASin(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprASin(X)))
					return Internal::ConstexprASin(X);
				else
					return Internal::RuntimeASin(X);
			}
#pragma endregion
#pragma region ACos
			forceinline constexpr Double ACos(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprACos(X)))
					return Internal::ConstexprACos(X);
				else
					return Internal::RuntimeACos(X);
			}
#pragma endregion
#pragma region ATan
			forceinline constexpr Double ATan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprATan(X)))
					return Internal::ConstexprATan(X);
				else
					return Internal::RuntimeATan(X);
			}
#pragma endregion
#pragma region ACtan
			forceinline constexpr Double ACtan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprACtan(X)))
					return Internal::ConstexprACtan(X);
				else
					return Internal::RuntimeACtan(X);
			}
#pragma endregion
#pragma region Sign
			forceinline constexpr Boolean Sign(Int8 Val)
			{
				return ((Val) & ((Seed::Int8)1 << 7)); //-V2533
			}
			forceinline constexpr Boolean Sign(Int16 Val)
			{
				return ((Val) & ((Seed::Int16)1 << 15)); //-V2533
			}
			forceinline constexpr Boolean Sign(Int32 Val)
			{
				return ((Val) & ((Seed::Int32)1 << 31)); //-V2533
			}
			forceinline constexpr Boolean Sign(Int64 Val)
			{
				return ((Val) & ((Seed::Int64)1 << 63)); //-V2533
			}
			forceinline constexpr Boolean Sign(Float Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSign(Val)))
					return Internal::ConstexprSign(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
			forceinline constexpr Boolean Sign(Double Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSign(Val)))
					return Internal::ConstexprSign(Val);
				else
					return Internal::RuntimeAbs(Val);
			}

			forceinline constexpr UInt8 GetNumberByPos(const UInt64 Number, const UInt8 Pos)
			{
				return static_cast<UInt8>(
					Number % Pow(10ULL, static_cast<UInt8>(Pos + 1ui8)) / Pow(10ULL, static_cast<UInt8>(Pos + 1ui8)) //-V747 //-V2517
					);
			}
#pragma endregion

			forceinline constexpr Double Exp(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprExp(X)))
					return Internal::ConstexprExp(X);
				else
					return Internal::RuntimeExp(X);
			}

			forceinline Double ModD(Double X, Double& Y) {
				return Internal::ModD(X, Y);
			}

			forceinline Float ModD(Float X, Float& Y) {
				return Internal::ModD(X, Y);
			}

			forceinline constexpr bool Equals(Float first, Float second){
				return Abs(first - second) <= Epsilon<Double>();
			}

			forceinline constexpr bool Equals(Double first, Double second){
				return Abs(first - second) <= Epsilon<Double>();
			}
		}
	}
}