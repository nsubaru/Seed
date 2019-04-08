#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			namespace Internal {
#pragma region Const
				__forceinline constexpr Double E()
				{
					return 2.71828182845904523536l;
				}
				__forceinline constexpr Double Pi()
				{
					return 3.141592653589793238462643383279502884197169399;
				}
#pragma endregion
#pragma region Abs

				__forceinline constexpr Seed::Int8 ConstexprAbs(Seed::Int8 Number)
				{
					return Number * ((Number < 0) * (-1i8) + (Number > 0));
				}
				static_assert((Int8)100 == ConstexprAbs((Int8)-100) && (Int8)100 == ConstexprAbs((Int8)100), "Модуль числа Int32 не коректно працює");

				__forceinline constexpr Seed::Int16 ConstexprAbs(Seed::Int16 Number)
				{
					return Number * ((Number < 0) * (-1i16) + (Number > 0));
				}
				static_assert((Int16)100 == ConstexprAbs((Int16)-100) && (Int16)100 == ConstexprAbs((Int16)100), "Модуль числа Int32 не коректно працює");

				__forceinline constexpr Seed::Int32 ConstexprAbs(Seed::Int32 Number)
				{
					return Number * ((Number < 0) * (-1) + (Number > 0));
				}
				static_assert(100 == ConstexprAbs(-100) && 100 == ConstexprAbs(100), "Модуль числа Int32 не коректно працює");

				__forceinline constexpr Seed::Int64 ConstexprAbs(Seed::Int64 Number)
				{
					return Number * ((Number < 0) * (-1i64) + (Number > 0));
				}
				static_assert(100L == ConstexprAbs(-100L) && 100L == ConstexprAbs(100L), "Модуль числа Int64 не коректно працює");

				__forceinline constexpr Seed::Float ConstexprAbs(Seed::Float Number)
				{
					if (Number < 0)
						return -Number;
					return Number;
				}

				__forceinline constexpr Seed::Double ConstexprAbs(Seed::Double Number)
				{
					if (Number < 0)
						return -Number;
					return Number;
				}
#pragma endregion
#pragma region Sign
				__forceinline constexpr Seed::Float ConstexprSign(Seed::Float Number)
				{
					return Number < 0;
				}

				__forceinline constexpr Seed::Double ConstexprSign(Seed::Double Number)
				{
					return Number < 0;
				}
#pragma endregion
#pragma region Factorial
				__forceinline constexpr UInt64 ConstexprFactorial(UInt64 N)
				{
					if (N == 1 || N == 0)
						return 1;
					return N * ConstexprFactorial(N - 1);
				}
#pragma endregion
#pragma region FEqual
				constexpr bool FEqual(Double x, Double y)
				{
					return ConstexprAbs(x - y) <= Math::Epsilon<Double>();
				}
#pragma endregion
#pragma region Exp
				__forceinline constexpr Double ConstexprExp(Double X, Double Res, Double N, Int64 i, Double t)
				{
					return FEqual(Res, Res + t / N) ?
						Res :
						ConstexprExp(X, Res + t / N, N * i, i + 1, t * X);
					return Res;
				}

				__forceinline constexpr Double ConstexprExp(Double X)
				{
					return ConstexprExp(X, 1, 1, 2, X);
				}
#pragma endregion
#pragma region Ln
				constexpr Double ConstexprLnIter(Double x, Double y)
				{
					return y + 2.0 * (x - ConstexprExp(y, 1, 1, 2, y)) / (x + ConstexprExp(y, 1, 1, 2, y));
				}
				constexpr Double ConstexprLog(Double x, Double y)
				{
					return FEqual(y, ConstexprLnIter(x, y)) ? y : ConstexprLog(x, ConstexprLnIter(x, y));
				}
				// if x <= 1, we will multiply by e^5 repeatedly until x > 1
				constexpr Double ConstexprLnGt(Double x)
				{
					return x > 0.25 ? ConstexprLog(x, 0) :
						ConstexprLnGt(x * E() * E() * E() * E() * E()) - 5;
				}
				// if x >= 2e10, we will divide by e^5 repeatedly until x < 2e10
				constexpr Double ConstexprLnLT(Double x)
				{
					return x < 1024 ? ConstexprLog(x, 0) :
						ConstexprLnLT(x / (E() * E() * E() * E() * E())) + 5;
				}

				constexpr Double ConstexprLn(Double X)
				{
					return
						X >= 1024 ? ConstexprLnLT(X) :
						ConstexprLnGt(X);
				}

				static_assert(FEqual(ConstexprLn(E()), 1), "ln not work properly");
#pragma endregion
#pragma region Other log
				constexpr Double ConstexprLog10(Double X)
				{
					return ConstexprLn(X) / ConstexprLn(10);
				}
				constexpr Double ConstexprLog2(Double X)
				{
					return ConstexprLn(X) / ConstexprLn(2);
				}
#pragma endregion
#pragma region Pow
				__forceinline constexpr UInt64 ConstexprPow(UInt64 X, UInt8 Y, UInt64 Res = 1)
				{
					for (UInt8 i = 0; i < Y; i++)
					{
						Res *= X;
					}
					return Res;
				}
				__forceinline constexpr Double ConstexprPow(Double X, Double Y)
				{
					return ConstexprExp(ConstexprLog(X, 0) * Y);
				}

				__forceinline constexpr Double ConstexprPow(Double X, Int32 Y)
				{
					return (Y == 0) ? 1 :
						Y == 1 ? X :
						Y > 1 ? ((Y & 1) ? X * ConstexprPow(X, Y - 1) : ConstexprPow(X, Y / 2) * ConstexprPow(X, Y / 2)) :
						1 / ConstexprPow(X, -Y);
				}
#pragma endregion
#pragma region Sqrt
				__forceinline constexpr Double ConstexprSqrt(Double X, Double Res = 0)
				{
					Res = X;
					for (int N = 1; N < 20; N++)
					{
						Res = 0.5 * (Res + X / Res);
					}
					return Res;
				}
#pragma endregion
#pragma region Sin
				__forceinline constexpr Double ConstexprSin(Double X, Double Res = 0)
				{
					for (int N = 0; N < 20; N++)
					{
						Res += (ConstexprPow(-1.0, N) / ConstexprFactorial(2 * N + 1)) * ConstexprPow(X, 2 * N + 1);
					}
					return Res;
				}
#pragma endregion
#pragma region Cos
				__forceinline constexpr Double ConstexprCos(Double X, Double Res = 0)
				{
					for (int N = 0; N < 15; N++)
					{
						Res += (ConstexprPow(-1.0, N) / ConstexprFactorial(2 * N)) * ConstexprPow(X, 2 * N);
					}
					return Res;
				}
				constexpr Double res = ConstexprSqrt(10);
#pragma endregion
#pragma region Tan
				__forceinline constexpr Double ConstexprTan(Double X)
				{
					return ConstexprSin(X) / ConstexprCos(X);
				}
#pragma endregion
#pragma region CTan
				__forceinline constexpr Double ConstexprCtan(Double X)
				{
					return ConstexprCos(X) / ConstexprSin(X);
				}
#pragma endregion
#pragma region ASin
				__forceinline constexpr Double ConstexprASin(Double X, Double Res = 0)
				{
					for (int N = 0; N <= 20; N++)
					{
						Res += (ConstexprFactorial(2 * N) / (ConstexprPow(4.0, N) * ConstexprPow(static_cast<Double>(ConstexprFactorial(N)), 2.0) * (2 * N + 1))) * ConstexprPow(X, 2 * N + 1);
					}
					return Res;
				}
#pragma endregion
#pragma region ACos
				__forceinline constexpr Double ConstexprACos(Double X)
				{
					return (Pi() / 2.0) - ConstexprASin(X);
				}
#pragma endregion
#pragma region ATan
				__forceinline constexpr Double ConstexprATan(Double X, Double Res = 0)
				{
					return ConstexprASin(1.0 / ConstexprSqrt(1 + X * X));
				}
#pragma endregion
#pragma region ACTan
				__forceinline constexpr Double ConstexprACtan(Double X)
				{
					return (Pi() / 2) - ConstexprATan(X);
				}
#pragma endregion

#pragma region Runtime
				__forceinline Float RuntimeAbs(Float Val)
				{
					union
					{
						Int32* I;
						Float* D;
					} D;
					D.D = &Val;
					*D.I &= 1 << 31;
					return Val;
				}

				__forceinline Double RuntimeAbs(Double Val)
				{
					union
					{
						Int64* I;
						Double* D;
					} D;
					D.D = &Val;
					*D.I &= 1ULL << 63;
					return Val;
				}

				__forceinline Seed::Boolean RuntimeSign(Seed::Float val)
				{
					static_assert(sizeof(Float) == sizeof(Int32), "Розміри Float і Int32 різні");
					union
					{
						Int32* I;
						Float* D;
					} D;
					D.D = &val;
					return ((*D.I) & ((Seed::Int32)1 << 31));
				}

				__forceinline Seed::Boolean RuntimeSign(Seed::Double val)
				{
					static_assert(sizeof(Double) == sizeof(Int64), "Розміри Float і Int32 різні");
					union
					{
						Int64* I;
						Double* D;
					} D;
					D.D = &val;
					return ((*D.I) & ((Seed::Int64)1 << 63));
				}

				Double RuntimePow(Double What, Double By);
				UInt64 RuntimePow(UInt64 What, UInt8 By);

				Double RuntimeLn(Double X);
				Double RuntimeLog10(Double X);
				Double RuntimeLog2(Double X);

				Double RuntimeExp(Double X);

				Double RuntimeSin(Double X);

				Double RuntimeCos(Double X);

				Double RuntimeTan(Double X);

				Double RuntimeCtan(Double X);

				Double RuntimeASin(Double X);

				Double RuntimeACos(Double X);

				Double RuntimeATan(Double X);

				Double RuntimeACtan(Double X);

				Double RuntimeSqrt(Double X);
#pragma endregion

				Float ModD(Float X, Float & intPart);

				Double ModD(Double X, Double & intPart);
			}
		}
	}
}