#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Math.Internal.h"
#include "Seed.Core.Math.SafeInt.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			//inline constexpr Double Pi = Details::Pi;

			//__forceinline constexpr Double Pow(Double What, Double By) {
			//	auto constexprLambda = [](Double What, Double Base, Double By) constexpr {
			//		return
			//	};
			//	if constexpr (IS_CONSTEXPR(constexprLambda))
			//		return constexprLambda(What, What, By);
			//	else
			//		return Details::RuntimePow(What, By);
			//}

			//constexpr Double res = Pow(6.0, 2.0);

			//__forceinline constexpr Float Pow(Float What, Float By) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprPow(What, What, By)))
			//		return Details::ConstexprPow(What, What, By);
			//	else
			//		return Details::RuntimePow(What, By);
			//}
			//__forceinline constexpr UInt64 Pow(UInt64 What, UInt8 By) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprPow(What, What, By)))
			//		return Details::ConstexprPow(What, What, By);
			//	else
			//		return Details::RuntimePow(What, By);
			//}

			//__forceinline constexpr Double Exp(Double By) {
			//	extern __forceinline constexpr Double ConstexprExp(Double X, Double Res);
			//	if constexpr (IS_CONSTEXPR(Seed::Core::Details::ConstexprExp(By)))
			//		return Details::ConstexprExp(By);
			//	else return Details::RuntimeExp(By);
			//}

			//constexpr Double expRes = Exp(50.0L);

			//__forceinline constexpr Double Sin(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprSin(Rad)))
			//		return Details::ConstexprSin(Rad);
			//	else
			//		return Details::RuntimeSin(Rad);
			//}

			//__forceinline constexpr Double Cos(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprCos(Rad)))
			//		return Details::ConstexprCos(Rad);
			//	else
			//		return Details::RuntimeCos(Rad);
			//}

			//__forceinline constexpr Double Tan(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprTan(Rad)))
			//		return Details::ConstexprTan(Rad);
			//	else
			//		return Details::RuntimeTan(Rad);
			//}

			//__forceinline constexpr Double Ctan(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprCtan(Rad)))
			//		return Details::ConstexprCtan(Rad);
			//	else
			//		return Details::RuntimeCtan(Rad);
			//}

			//__forceinline constexpr Double ACos(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprACos(Rad)))
			//		return Details::ConstexprACos(Rad);
			//	else
			//		return Details::RuntimeACos(Rad);
			//}

			//__forceinline constexpr Double ASin(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprASin(Rad)))
			//		return Details::ConstexprASin(Rad);
			//	else
			//		return Details::RuntimeASin(Rad);
			//}

			//__forceinline constexpr Double ATan(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprATan(Rad)))
			//		return Details::ConstexprATan(Rad);
			//	else
			//		return Details::RuntimeATan(Rad);
			//}

			//__forceinline constexpr Double ACtan(Double Rad) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprACtan(Rad)))
			//		return Details::ConstexprCtan(Rad);
			//	else
			//		return Details::RuntimeACtan(Rad);
			//}

			//__forceinline constexpr Double Sqrt(Double Val) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprSqrt(Val)))
			//		return Details::ConstexprSqrt(Val);
			//	else
			//		return Details::RuntimeCtan(Val);
			//}

			//__forceinline constexpr Int8 Abs(Int8 Val) {
			//	return Details::ConstexprAbs(Val);
			//}

			//__forceinline constexpr Int16 Abs(Int16 Val) {
			//	return Details::ConstexprAbs(Val);
			//}
			//__forceinline constexpr Int32 Abs(Int32 Val) {
			//	return Details::ConstexprAbs(Val);
			//}
			//__forceinline constexpr Int64 Abs(Int64 Val) {
			//	return Details::ConstexprAbs(Val);
			//}
			//__forceinline constexpr Float Abs(Float Val) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprAbs(Val)))
			//		return Details::ConstexprAbs(Val);
			//	else
			//		return Details::RuntimeAbs(Val);
			//}

			//__forceinline constexpr Double Abs(Double Val) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprAbs(Val)))
			//		return Details::ConstexprAbs(Val);
			//	else
			//		return Details::RuntimeAbs(Val);
			//}

			//__forceinline constexpr Boolean Sign(Int8 Val) {
			//	return ((Val) & ((Seed::Int8)1 << 7));
			//}
			//__forceinline constexpr Boolean Sign(Int16 Val) {
			//	return ((Val) & ((Seed::Int16)1 << 15));
			//}
			//__forceinline constexpr Boolean Sign(Int32 Val) {
			//	return ((Val) & ((Seed::Int32)1 << 31));
			//}
			//__forceinline constexpr Boolean Sign(Int64 Val) {
			//	return ((Val) & ((Seed::Int64)1 << 63));
			//}
			//__forceinline constexpr Boolean Sign(Float Val) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprSign(Val)))
			//		return Details::ConstexprSign(Val);
			//	else
			//		return Details::RuntimeAbs(Val);
			//}
			//__forceinline constexpr Boolean Sign(Double Val) {
			//	if constexpr (IS_CONSTEXPR(Details::ConstexprSign(Val)))
			//		return Details::ConstexprSign(Val);
			//	else
			//		return Details::RuntimeAbs(Val);
			//}

			//__forceinline constexpr Boolean GetNumberByPos(const UInt64 Number, const UInt8 Pos) {
			//	return Number % Pow(10ULL, Pos + 1) / Pow(10ULL, Pos + 1);
			//}

			//__forceinline constexpr Float Ceil(Float Val) { return Details::Ceil(Val); }
			//__forceinline constexpr Double Ceil(Double Val) { return Details::Ceil(Val); }

			//__forceinline constexpr Float Floor(Float Val) { return Details::Floor(Val); }
			//__forceinline constexpr Double Floor(Double Val) { return Details::Floor(Val); }

			//__forceinline constexpr Double ModD(Double Val, Double & IntPart) { return Details::ModD(Val, IntPart); }
			//__forceinline constexpr Float ModD(Float Val, Float & IntPart) { return Details::ModD(Val, IntPart); }

			//__forceinline constexpr Float Log2(Float X);
			//__forceinline constexpr Double Log2(Double X);

			//__forceinline constexpr Float Log10(Float X);
			//__forceinline constexpr Double Log10(Double X);

			//__forceinline constexpr Float Log(Float Base, Float X);
			//__forceinline constexpr Double Log(Double Base, Double X);
#pragma region Abs
			__forceinline constexpr Int8 Abs(Int8 Val)
			{
				return Internal::ConstexprAbs(Val);
			}

			__forceinline constexpr Int16 Abs(Int16 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			__forceinline constexpr Int32 Abs(Int32 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			__forceinline constexpr Int64 Abs(Int64 Val)
			{
				return Internal::ConstexprAbs(Val);
			}
			__forceinline constexpr Float Abs(Float Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprAbs(Val)))
					return Internal::ConstexprAbs(Val);
				else
					return Internal::RuntimeAbs(Val);
			}

			__forceinline constexpr Double Abs(Double Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprAbs(Val)))
					return Internal::ConstexprAbs(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
#pragma endregion
#pragma region Pow
			__forceinline constexpr UInt64 Pow(UInt64 X, UInt8 Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
			__forceinline constexpr Double Pow(Double X, Int32 Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
			__forceinline constexpr Double Pow(Double X, Double Y)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprPow(X, Y)))
					return Internal::ConstexprPow(X, Y);
				else
					return Internal::RuntimePow(X, Y);
			}
#pragma endregion
#pragma region Log
			__forceinline constexpr Double Ln(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLn(X)))
					return Internal::ConstexprLn(X);
				else
					return Internal::RuntimeLn(X);
			}
			__forceinline constexpr Double Lg(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLn(X)))
					return Internal::ConstexprLn(X);
				else
					return Internal::RuntimeLn(X);
			}
			__forceinline constexpr Double Log2(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprLog2(X)))
					return Internal::ConstexprLog2(X);
				else
					return Internal::RuntimeLog2(X);
			}
#pragma endregion
#pragma region Sqrt
			__forceinline constexpr Double Sqrt(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSqrt(X)))
					return Internal::ConstexprSqrt(X);
				else
					return Internal::RuntimeSqrt(X);
			}
#pragma endregion

#pragma region Sin
			__forceinline constexpr Double Sin(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSin(X)))
					return Internal::ConstexprSin(X);
				else
					return Internal::RuntimeSin(X);
			}
#pragma endregion

#pragma region Cos
			__forceinline constexpr Double Cos(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprCos(X)))
					return Internal::ConstexprCos(X);
				else
					return Internal::RuntimeCos(X);
			}
#pragma endregion
#pragma region Tan
			__forceinline constexpr Double Tan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprTan(X)))
					return Internal::ConstexprTan(X);
				else
					return Internal::RuntimeTan(X);
			}
#pragma endregion
#pragma region CTan
			__forceinline constexpr Double CTan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprCtan(X)))
					return Internal::ConstexprCtan(X);
				else
					return Internal::RuntimeCtan(X);
			}
#pragma endregion
#pragma region ASin
			__forceinline constexpr Double ASin(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprASin(X)))
					return Internal::ConstexprASin(X);
				else
					return Internal::RuntimeASin(X);
			}
#pragma endregion
#pragma region ACos
			__forceinline constexpr Double ACos(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprACos(X)))
					return Internal::ConstexprACos(X);
				else
					return Internal::RuntimeACos(X);
			}
#pragma endregion
#pragma region ATan
			__forceinline constexpr Double ATan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprATan(X)))
					return Internal::ConstexprATan(X);
				else
					return Internal::RuntimeATan(X);
			}
#pragma endregion
#pragma region ACtan
			__forceinline constexpr Double ACtan(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprACtan(X)))
					return Internal::ConstexprACtan(X);
				else
					return Internal::RuntimeACtan(X);
			}
#pragma endregion
#pragma region Sign
			__forceinline constexpr Boolean Sign(Int8 Val)
			{
				return ((Val) & ((Seed::Int8)1 << 7));
			}
			__forceinline constexpr Boolean Sign(Int16 Val)
			{
				return ((Val) & ((Seed::Int16)1 << 15));
			}
			__forceinline constexpr Boolean Sign(Int32 Val)
			{
				return ((Val) & ((Seed::Int32)1 << 31));
			}
			__forceinline constexpr Boolean Sign(Int64 Val)
			{
				return ((Val) & ((Seed::Int64)1 << 63));
			}
			__forceinline constexpr Boolean Sign(Float Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSign(Val)))
					return Internal::ConstexprSign(Val);
				else
					return Internal::RuntimeAbs(Val);
			}
			__forceinline constexpr Boolean Sign(Double Val)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprSign(Val)))
					return Internal::ConstexprSign(Val);
				else
					return Internal::RuntimeAbs(Val);
			}

			__forceinline constexpr Boolean GetNumberByPos(const UInt64 Number, const UInt8 Pos)
			{
				return Number % Pow(10ULL, static_cast<UInt8>(Pos + 1ui8)) / (10ULL, static_cast<UInt8>(Pos + 1ui8));
			}
#pragma endregion

			__forceinline constexpr Double Exp(Double X)
			{
				if constexpr (IS_CONSTEXPR(Internal::ConstexprExp(X)))
					return Internal::ConstexprExp(X);
				else
					return Internal::RuntimeExp(X);
			}

			__forceinline Double ModD(Double X, Double& Y) {
				return Internal::ModD(X, Y);
			}

			__forceinline Float ModD(Float X, Float& Y) {
				return Internal::ModD(X, Y);
			}
		}
	}
}