#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Math.Imp.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			inline constexpr Double Pi = Details::Pi;

			__forceinline constexpr Double Pow(Double What, Double By) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprPow(What, What, By)))
					return Details::ConstexprPow(What, What, By);
				else
					return Details::RuntimePow(What, By);
			}
			__forceinline constexpr Float Pow(Float What, Float By) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprPow(What, What, By)))
					return Details::ConstexprPow(What, What, By);
				else
					return Details::RuntimePow(What, By);
			}
			__forceinline constexpr UInt64 Pow(UInt64 What, UInt8 By) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprPow(What, What, By)))
					return Details::ConstexprPow(What, What, By);
				else
					return Details::RuntimePow(What, By);
			}

			__forceinline constexpr Double Exp(Double By) {
				if constexpr (IS_CONSTEXPR(Seed::Core::Details::ConstexprExp(By)))
					return Details::ConstexprExp(By);
				else return Details::RuntimeExp(By);
			}

			__forceinline constexpr Double Sin(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprSin(Rad)))
					return Details::ConstexprSin(Rad);
				else
					return Details::RuntimeSin(Rad);
			}


			__forceinline constexpr Double Cos(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprCos(Rad)))
					return Details::ConstexprCos(Rad);
				else
					return Details::RuntimeCos(Rad);
			}

			__forceinline constexpr Double Tan(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprTan(Rad)))
					return Details::ConstexprTan(Rad);
				else
					return Details::RuntimeTan(Rad);
			}

			__forceinline constexpr Double Ctan(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprCtan(Rad)))
					return Details::ConstexprCtan(Rad);
				else
					return Details::RuntimeCtan(Rad);
			}

			__forceinline constexpr Double ACos(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprACos(Rad)))
					return Details::ConstexprACos(Rad);
				else
					return Details::RuntimeACos(Rad);
			}

			__forceinline constexpr Double ASin(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprASin(Rad)))
					return Details::ConstexprASin(Rad);
				else
					return Details::RuntimeASin(Rad);
			}

			__forceinline constexpr Double ATan(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprATan(Rad)))
					return Details::ConstexprATan(Rad);
				else
					return Details::RuntimeATan(Rad);
			}

			__forceinline constexpr Double ACtan(Double Rad) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprACtan(Rad)))
					return Details::ConstexprCtan(Rad);
				else
					return Details::RuntimeACtan(Rad);
			}

			__forceinline constexpr Double Sqrt(Double Val) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprSqrt(Val)))
					return Details::ConstexprSqrt(Val);
				else
					return Details::RuntimeCtan(Val);
			}

			__forceinline constexpr Int8 Abs(Int8 Val) {
				return Details::ConstexprAbs(Val);
			}

			__forceinline constexpr Int16 Abs(Int16 Val) {
				return Details::ConstexprAbs(Val);
			}
			__forceinline constexpr Int32 Abs(Int32 Val) {
				return Details::ConstexprAbs(Val);
			}
			__forceinline constexpr Int64 Abs(Int64 Val) {
				return Details::ConstexprAbs(Val);
			}
			__forceinline constexpr Float Abs(Float Val) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprAbs(Val)))
					return Details::ConstexprAbs(Val);
				else
					return Details::RuntimeAbs(Val);
			}

			__forceinline constexpr Double Abs(Double Val) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprAbs(Val)))
					return Details::ConstexprAbs(Val);
				else
					return Details::RuntimeAbs(Val);
			}

			__forceinline constexpr Boolean Sign(Int8 Val) {
				return ((Val) & ((Seed::Int8)1 << 7));
			}
			__forceinline constexpr Boolean Sign(Int16 Val) {
				return ((Val) & ((Seed::Int16)1 << 15));
			}
			__forceinline constexpr Boolean Sign(Int32 Val) {
				return ((Val) & ((Seed::Int32)1 << 31));
			}
			__forceinline constexpr Boolean Sign(Int64 Val) {
				return ((Val) & ((Seed::Int64)1 << 63));
			}
			__forceinline constexpr Boolean Sign(Float Val) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprSign(Val)))
					return Details::ConstexprSign(Val);
				else
					return Details::RuntimeAbs(Val);
			}
			__forceinline constexpr Boolean Sign(Double Val) {
				if constexpr (IS_CONSTEXPR(Details::ConstexprSign(Val)))
					return Details::ConstexprSign(Val);
				else
					return Details::RuntimeAbs(Val);
			}
			
			__forceinline constexpr Boolean GetNumberByPos(const UInt64 Number, const UInt8 Pos) {
				return Number % Pow(10ULL, Pos + 1) / Pow(10ULL, Pos + 1);
			}

			__forceinline constexpr Float Ceil(Float Val) { return Details::Ceil(Val); }
			__forceinline constexpr Double Ceil(Double Val) { return Details::Ceil(Val); }

			__forceinline constexpr Float Floor(Float Val) { return Details::Floor(Val); }
			__forceinline constexpr Double Floor(Double Val) { return Details::Floor(Val); }

			__forceinline constexpr Double ModD(Double Val, Double & IntPart) { return Details::ModD(Val, IntPart); }
			__forceinline constexpr Float ModD(Float Val, Float & IntPart) { return Details::ModD(Val, IntPart); }

			__forceinline constexpr Float Log2(Float X);
			__forceinline constexpr Double Log2(Double X);

			__forceinline constexpr Float Log10(Float X);
			__forceinline constexpr Double Log10(Double X);

			__forceinline constexpr Float Log(Float Base, Float X);
			__forceinline constexpr Double Log(Double Base, Double X);
		}
	}
}