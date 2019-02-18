#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Details {
			__forceinline constexpr Seed::Int8 ConstexprAbs(Seed::Int8 Number) { return Number * ((Number < 0) * (-1i8) + (Number > 0)); }
			static_assert((Int8)100 == ConstexprAbs((Int8)-100) && (Int8)100 == ConstexprAbs((Int8)100), "Модуль числа Int32 не коректно працює");

			__forceinline constexpr Seed::Int16 ConstexprAbs(Seed::Int16 Number) { return Number * ((Number < 0) * (-1i16) + (Number > 0)); }
			static_assert((Int16)100 == ConstexprAbs((Int16)-100) && (Int16)100 == ConstexprAbs((Int16)100), "Модуль числа Int32 не коректно працює");

			__forceinline constexpr Seed::Int32 ConstexprAbs(Seed::Int32 Number) { return Number * ((Number < 0) * (-1) + (Number > 0)); }
			static_assert(100 == ConstexprAbs(-100) && 100 == ConstexprAbs(100), "Модуль числа Int32 не коректно працює");

			__forceinline constexpr Seed::Int64 ConstexprAbs(Seed::Int64 Number) { return Number * ((Number < 0) * (-1i64) + (Number > 0)); }
			static_assert(100L == ConstexprAbs(-100L) && 100L == ConstexprAbs(100L), "Модуль числа Int64 не коректно працює");

			__forceinline constexpr Seed::Float ConstexprAbs(Seed::Float Number) {
				if (Number < 0)
					return -Number;
				return Number;
			}

			__forceinline constexpr Seed::Double ConstexprAbs(Seed::Double Number) {
				if (Number < 0)
					return -Number;
				return Number;
			}

			__forceinline constexpr Seed::Float ConstexprSign(Seed::Float Number) {
				return Number < 0;
			}

			__forceinline constexpr Seed::Double ConstexprSign(Seed::Double Number) {
				return Number < 0;
			}

			__forceinline constexpr Double ConstexprPow(Double What, Double Base, Double By) {
				if (By == 1)
					return What;
				if (By == 0)
					return 1;
				return ConstexprPow(What*Base, Base, By - 1);
			}

			__forceinline constexpr Float ConstexprPow(Float What, Float Base, Float By) {
				if (By == 1)
					return What;
				if (By == 0)
					return 1;
				return ConstexprPow(What*Base, Base, By - 1);
			}

			__forceinline constexpr UInt64 ConstexprPow(UInt64 What, UInt64 Base, UInt8 By) {
				if (By == 1)
					return What;
				if (By == 0)
					return 1;
				return ConstexprPow(What*Base, Base, By - 1);
			}

			__forceinline constexpr UInt64 ConstexprFactorial(UInt64 N) {
				if (N == 1 || N == 0)
					return 1;
				return N * ConstexprFactorial(N - 1);
			}

			__forceinline constexpr Double ConstexprDownstreamFactorial(Double X, Double N, Double Res = 1) {
				for (int k = 0; k < 20; k++) {
					Res *= (X - k + 1);
				}
				return Res;
			}

			__forceinline constexpr Double ConstexprSqrt(Double X, Double Res = 0) {
				Res = X;
				for (int N = 1; N < 20; N++) {
					Res = 0.5 * (Res + X / Res);
				}
				return Res;
			}

			__forceinline constexpr Double ConstexprPi(Double A = 1, Double  B = 1 / ConstexprSqrt(2), Double T = 1 / 4.0, Double P = 1, Double Res = 0) {
				while (ConstexprAbs(A - B) != 0) {
					Res = A;
					A = (Res + B) / 2.0;
					B = ConstexprSqrt(Res * B);
					T = T - P * ConstexprPow(A - Res, A - Res, 2);
					P = 2 * P;
				}
				Res = ConstexprPow(A + B, A + B, 2) / (4 * T);
				return Res;
			}

			inline constexpr Double Pi = 3.141592653589793238462643383279502884197169399;

			__forceinline constexpr Double ConstexprExp(Double X, Double Res = 0) {
				Res = 1.0 + X;
				for (int N = 2; N < 20; N++) {
					Res += ConstexprPow(X, X, N) / ConstexprFactorial(N);
				}
				return Res;
			}

			__forceinline constexpr Double ConstexprSin(Double X, Double Res = 0) {
				for (int N = 0; N < 20; N++) {
					Res += (ConstexprPow(-1.0, -1.0, N) / ConstexprFactorial(2 * N + 1)) * ConstexprPow(X, X, 2 * N + 1);
				}
				return Res;
			}

			__forceinline constexpr Double ConstexprCos(Double X, Double Res = 0) {
				for (int N = 0; N < 15; N++) {
					Res += (ConstexprPow(-1.0, -1.0, N) / ConstexprFactorial(2 * N))*ConstexprPow(X, X, 2 * N);
				}
				return Res;
			}

			__forceinline constexpr Double ConstexprTan(Double X) {
				return ConstexprSin(X) / ConstexprCos(X);
			}

			__forceinline constexpr Double ConstexprCtan(Double X) {
				return ConstexprCos(X) / ConstexprSin(X);
			}

			__forceinline constexpr Double ConstexprASin(Double X, Double Res = 0) {
				for (int N = 0; N <= 20; N++) {
					Res += (ConstexprFactorial(2 * N) / (ConstexprPow(4.0, 4.0, N)*ConstexprPow(ConstexprFactorial(N), ConstexprFactorial(N), 2)*(2 * N + 1)))*ConstexprPow(X, X, 2 * N + 1);
				}
				return Res;
			}

			__forceinline constexpr Double ConstexprACos(Double X) {
				return (Pi / 2.0) - ConstexprASin(X);
			}

			__forceinline constexpr Double ConstexprATan(Double X, Double Res = 0) {
				return ConstexprASin(1.0 / ConstexprSqrt(1 + X * X));
			}

			__forceinline constexpr Double ConstexprACtan(Double X) {
				return (Pi / 2) - ConstexprATan(X);
			}

			Double RuntimePow(Double What, Double By);
			Float RuntimePow(Float What, Float By);
			UInt64 RuntimePow(UInt64 What, UInt8 By);

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

			__forceinline Float RuntimeAbs(Float Val) {
				union {
					Int32 * I;
					Float * D;
				} D;
				D.D = &Val;
				*D.I &= 1 << 31;
				return Val;
			}

			__forceinline Double RuntimeAbs(Double Val) {
				union {
					Int64 *  I;
					Double * D;
				} D;
				D.D = &Val;
				*D.I &= 1ULL << 63;
				return Val;
			}

			__forceinline Seed::Boolean RuntimeSign(Seed::Float val) {
				static_assert(sizeof(Float) == sizeof(Int32), "Розміри Float і Int32 різні");
				union
				{
					Int32 *  I;
					Float * D;
				} D;
				D.D = &val;
				return ((*D.I) & ((Seed::Int32)1 << 31));
			}

			__forceinline Seed::Boolean RuntimeSign(Seed::Double val) {
				static_assert(sizeof(Double) == sizeof(Int64), "Розміри Float і Int32 різні");
				union
				{
					Int64 *  I;
					Double * D;
				} D;
				D.D = &val;
				return ((*D.I) & ((Seed::Int64)1 << 63));
			}

			__forceinline constexpr Seed::Int64 ConstexprCeil(Seed::Float X) {
				return ConstexprPow(static_cast<Double>(2L), static_cast<Double>(2L), static_cast<Double>((sizeof(Seed::Int64) / 2) * 8)) -
					Int64(ConstexprPow(static_cast<Double>(2L), static_cast<Double>(2L), (sizeof(Seed::Int64) / 2) * 8) - X);
			}
			static_assert(250L == ConstexprCeil(250.0f), "ConstexprCeil not work properly");
			static_assert(251L == ConstexprCeil(250.5f), "ConstexprCeil not work properly");

			__forceinline constexpr Seed::Int64 ConstexprFloor(Seed::Float X) {
				return Int64(X + ConstexprPow(static_cast<Double>(2L), static_cast<Double>(2L), (sizeof(Seed::Int64) / 2) * 8)) -
					ConstexprPow(static_cast<Double>(2L), static_cast<Double>(2L), (sizeof(Seed::Int64) / 2) * 8);
			}

			static_assert(250L == ConstexprFloor(250.0f), "ConstexprFloor not work properly");
			static_assert(250L == ConstexprFloor(250.5f), "ConstexprFloor not work properly");

			Seed::Float Ceil(Seed::Float X);
			Seed::Double Ceil(Seed::Double X);

			Seed::Float Floor(Seed::Float X);
			Seed::Double Floor(Seed::Double X);

			Seed::Float ModD(Seed::Float In, Seed::Float & Part);
			Seed::Double ModD(Seed::Double In, Seed::Double & Part);
		}
	}
}