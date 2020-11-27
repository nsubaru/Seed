#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			namespace Internal {
#pragma region Const
				ForceInline constexpr Double E()
				{
					return 2.71828182845904523536l; //-V2517
				}
				ForceInline constexpr Double Pi()
				{
					return 3.141592653589793238462643383279502884197169399;
				}
#pragma endregion
#pragma region Abs

				ForceInline constexpr Seed::Int8 ConstexprAbs(Seed::Int8 Number)
				{
					return Number * ((Number < 0) * (-1i8) + (Number > 0)); //-V2517
				}
				static_assert((Int8)100 == ConstexprAbs((Int8)-100) && (Int8)100 == ConstexprAbs((Int8)100), "Модуль числа Int32 не коректно працює"); //-V2533

				ForceInline constexpr Seed::Int16 ConstexprAbs(Seed::Int16 Number)
				{
					return Number * ((Number < 0) * (-1i16) + (Number > 0)); //-V2517
				}
				static_assert((Int16)100 == ConstexprAbs((Int16)-100) && (Int16)100 == ConstexprAbs((Int16)100), "Модуль числа Int32 не коректно працює"); //-V2533

				ForceInline constexpr Seed::Int32 ConstexprAbs(Seed::Int32 Number)
				{
					return Number * ((Number < 0) * (-1) + (Number > 0));
				}
				static_assert(100 == ConstexprAbs(-100) && 100 == ConstexprAbs(100), "Модуль числа Int32 не коректно працює");

				ForceInline constexpr Seed::Int64 ConstexprAbs(Seed::Int64 Number)
				{
					return Number * ((Number < 0) * (-1i64) + (Number > 0)); //-V2517
				}
				static_assert(100L == ConstexprAbs((Int64)-100L) && 100L == ConstexprAbs((Int64) 100L), "Модуль числа Int64 не коректно працює");

				ForceInline constexpr Seed::Float ConstexprAbs(Seed::Float Number)
				{
					if (Number < 0)
						return -Number;
					return Number;
				}

				ForceInline constexpr Seed::Double ConstexprAbs(Seed::Double Number)
				{
					if (Number < 0)
						return -Number;
					return Number;
				}
#pragma endregion
#pragma region Sign
				ForceInline constexpr Boolean ConstexprSign(Seed::Float Number)
				{
					return Number < 0;
				}

				ForceInline constexpr Boolean ConstexprSign(Seed::Double Number)
				{
					return Number < 0;
				}
#pragma endregion
#pragma region Factorial
				ForceInline constexpr UInt64 ConstexprFactorial(UInt64 N)
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
				ForceInline constexpr Double ConstexprExp(Double X, Double Res, Double N, Int64 i, Double t)
				{
					return FEqual(Res, Res + t / N) ?
						Res :
						ConstexprExp(X, Res + t / N, N * i, i + 1, t * X);
					return Res;
				}

				ForceInline constexpr Double ConstexprExp(Double X)
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
				ForceInline constexpr UInt64 ConstexprPow(UInt64 X, UInt8 Y, UInt64 Res = 1)
				{
					for (UInt8 i = 0; i < Y; i++)
					{
						Res *= X;
					}
					return Res;
				}
				ForceInline constexpr Double ConstexprPow(Double X, Double Y)
				{
					return ConstexprExp(ConstexprLog(X, 0) * Y);
				}

				ForceInline constexpr Double ConstexprPow(Double X, Int32 Y)
				{
					return (Y == 0) ? 1 :
						Y == 1 ? X :
						Y > 1 ? ((Y & 1) ? X * ConstexprPow(X, Y - 1) : ConstexprPow(X, Y / 2) * ConstexprPow(X, Y / 2)) :
						1 / ConstexprPow(X, -Y);
				}
#pragma endregion
#pragma region Sqrt
				ForceInline constexpr Double ConstexprSqrt(Double X, Double Res = 0)
				{
					Res = X; //-V763
					for (int N = 1; N < 20; N++)
					{
						Res = 0.5 * (Res + X / Res);
					}
					return Res;
				}
#pragma endregion
#pragma region Sin
				ForceInline constexpr Double ConstexprSin(Double X, Double Res = 0)
				{
					for (int N = 0; N < 20; N++)
					{
						Res += (ConstexprPow(-1.0, N) / ConstexprFactorial(2ull * N + 1)) * ConstexprPow(X, 2 * N + 1);
					}
					return Res;
				}
#pragma endregion
#pragma region Cos
				ForceInline constexpr Double ConstexprCos(Double X, Double Res = 0)
				{
					for (int N = 0; N < 15; N++)
					{
						Res += (ConstexprPow(-1.0, N) / ConstexprFactorial(2ull * N)) * ConstexprPow(X, 2 * N);
					}
					return Res;
				}
				constexpr Double res = ConstexprSqrt(10);
#pragma endregion
#pragma region Tan
				ForceInline constexpr Double ConstexprTan(Double X)
				{
					return ConstexprSin(X) / ConstexprCos(X);
				}
#pragma endregion
#pragma region CTan
				ForceInline constexpr Double ConstexprCtan(Double X)
				{
					return ConstexprCos(X) / ConstexprSin(X);
				}
#pragma endregion
#pragma region ASin
				ForceInline constexpr Double ConstexprASin(Double X, Double Res = 0)
				{
					for (int N = 0; N <= 20; N++)
					{
						Res += (ConstexprFactorial(2ull * N) / (ConstexprPow(4.0, N) * ConstexprPow(static_cast<Double>(ConstexprFactorial(N)), 2.0) * (2ull * N + 1))) * ConstexprPow(X, 2 * N + 1);
					}
					return Res;
				}
#pragma endregion
#pragma region ACos
				ForceInline constexpr Double ConstexprACos(Double X)
				{
					return (Pi() / 2.0) - ConstexprASin(X);
				}
#pragma endregion
#pragma region ATan
				ForceInline constexpr Double ConstexprATan(Double X, Double Res = 0) //-V2537
				{
					return ConstexprASin(1.0 / ConstexprSqrt(1 + X * X));
				}
#pragma endregion
#pragma region ACTan
				ForceInline constexpr Double ConstexprACtan(Double X)
				{
					return (Pi() / 2) - ConstexprATan(X);
				}
#pragma endregion

#pragma region Runtime
				ForceInline Float RuntimeAbs(Float Val)
				{
					*(((int*) &Val)) &= 0x7fffffff;
					return Val;
				}

				ForceInline Double RuntimeAbs(Double Val)
				{
					*(((int*) &Val) + 1) &= 0x7fffffff;
					return Val;
				}

				ForceInline Seed::Boolean RuntimeSign(Seed::Float val)
				{
					static_assert(sizeof(Float) == sizeof(Int32), "Розміри Float і Int32 різні");
					union
					{
						Int32* I;
						Float* D;
					} D;
					D.D = &val;
					return ((*D.I) & ((Seed::Int32)1 << 31)); //-V2533
				}

				ForceInline Seed::Boolean RuntimeSign(Seed::Double val)
				{
					static_assert(sizeof(Double) == sizeof(Int64), "Розміри Float і Int32 різні");
					union
					{
						Int64* I;
						Double* D;
					} D;
					D.D = &val;
					return ((*D.I) & ((Seed::Int64)1 << 63)); //-V2533
				}

				Double SeedKernelCall RuntimePow(Double What, Double By);
				UInt64 SeedKernelCall RuntimePow(UInt64 What, UInt8 By);

				Double SeedKernelCall RuntimeLn(Double X);
				Double SeedKernelCall RuntimeLog10(Double X);
				Double SeedKernelCall RuntimeLog2(Double X);

				Double SeedKernelCall RuntimeExp(Double X);

				Double SeedKernelCall RuntimeSin(Double X);

				Double SeedKernelCall RuntimeCos(Double X);

				Double SeedKernelCall RuntimeTan(Double X);

				Double SeedKernelCall RuntimeCtan(Double X);

				Double SeedKernelCall RuntimeASin(Double X);

				Double SeedKernelCall RuntimeACos(Double X);

				Double SeedKernelCall RuntimeATan(Double X);

				Double SeedKernelCall RuntimeACtan(Double X);

				Double SeedKernelCall RuntimeSqrt(Double X);
#pragma endregion

				Float SeedKernelCall ModD(Float X, Float& intPart);

				Double SeedKernelCall ModD(Double X, Double& intPart);
			}
		}
	}
}