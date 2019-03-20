#include "Seed.Core.Math.Internal.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

namespace Seed {
	inline namespace Core {
		namespace Math {
			namespace Internal {
#pragma warning(push)
#pragma warning(disable: 4162)
#pragma intrinsic(pow)
				Double RuntimePow(Double What, Double By)
				{
					return pow(What, By);
				}

#pragma intrinsic(pow)
				Float RuntimePow(Float What, Float By)
				{
					return (Float)pow(What, By);
				}

				UInt64 RuntimePow(UInt64 What, UInt8 By)
				{
					constexpr UInt8 highest_bit_set[] = {
								0, 1, 2, 2, 3, 3, 3, 3,
								4, 4, 4, 4, 4, 4, 4, 4,
								5, 5, 5, 5, 5, 5, 5, 5,
								5, 5, 5, 5, 5, 5, 5, 5,
								6, 6, 6, 6, 6, 6, 6, 6,
								6, 6, 6, 6, 6, 6, 6, 6,
								6, 6, 6, 6, 6, 6, 6, 6,
								6, 6, 6, 6, 6, 6, 6, 255,
								//все що має 1 в 63 біті при піднесені в степінь What > 0 гарантовано вийде за межі UInt64
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
								255, 255, 255, 255, 255, 255, 255, 255,
					};

					UInt64 result = 1;

					switch (highest_bit_set[By])
					{
					case 255: //ми можем використовувати 255 як маркер переповнення
						if (What == 1)
						{
							return 1;
						}

						if (What == -1)
						{
							return 1 - 2 * (By & 1);
						}

						return 0;
					case 6:
						if (By & 1) result *= What;
						By >>= 1;
						What *= What;
					case 5:
						if (By & 1) result *= What;
						By >>= 1;
						What *= What;
					case 4:
						if (By & 1) result *= What;
						By >>= 1;
						What *= What;
					case 3:
						if (By & 1) result *= What;
						By >>= 1;
						What *= What;
					case 2:
						if (By & 1) result *= What;
						By >>= 1;
						What *= What;
					case 1:
						if (By & 1) result *= What;
					default:
						return result;
					}
				}
#pragma intrinsic(log)
				Double log10(Double X)
				{
					return log(X) / log(10);
				}
#pragma intrinsic(log)
				Double RuntimeLn(Double X)
				{
					return log(X);
				}
#pragma intrinsic(log10)
				Double RuntimeLog10(Double X)
				{
					return log10(X);
				}
#pragma intrinsic(log)
				Double RuntimeLog2(Double X)
				{
					return log(X) / log(2);
				}

				Double exp(Double X)
				{
					return 1.0 + X +
						RuntimePow(X, 2) / ConstexprFactorial(2) +
						RuntimePow(X, 3) / ConstexprFactorial(3) +
						RuntimePow(X, 4) / ConstexprFactorial(4) +
						RuntimePow(X, 5) / ConstexprFactorial(5) +
						RuntimePow(X, 6) / ConstexprFactorial(6) +
						RuntimePow(X, 7) / ConstexprFactorial(7) +
						RuntimePow(X, 8) / ConstexprFactorial(8) +
						RuntimePow(X, 9) / ConstexprFactorial(9);
				}

#pragma intrinsic(exp)
				Double RuntimeExp(Double X)
				{
					return exp(X);
				}

#pragma intrinsic(sin)
				Double RuntimeSin(Double X)
				{
					return sin(X);
				}

#pragma intrinsic(cos)
				Double RuntimeCos(Double X)
				{
					return cos(X);
				}

#pragma intrinsic(tan)
				Double RuntimeTan(Double X)
				{
					return tan(X);
				}

#pragma intrinsic(cos)
#pragma intrinsic(sin)
				Double RuntimeCtan(Double X)
				{
					return sin(X) / cos(X);
				}

#pragma intrinsic(asin)
				Double RuntimeASin(Double X)
				{
					return asin(X);
				}

				Double RuntimeACos(Double X)
				{
					return (Seed::Core::Math::Internal::Pi() / 2) - asin(X);
				}

#pragma intrinsic(atan)
				Double RuntimeATan(Double X)
				{
					return atan(X);
				}

#pragma intrinsic(atan)
				Double RuntimeACtan(Double X)
				{
					return (Seed::Core::Math::Internal::Pi() / 2) - atan(X);
				}

#pragma intrinsic(sqrt)
				Double RuntimeSqrt(Double X)
				{
					return sqrt(X);
				}

				Seed::Float Ceil(Seed::Float X)
				{
					return static_cast<Float>(ceil(X));
				}

				Seed::Double Ceil(Seed::Double X)
				{
					return ceil(X);
				}

				Seed::Float Floor(Seed::Float X)
				{
					return static_cast<Float>(floor(X));
				}

				Seed::Double Floor(Seed::Double X)
				{
					return floor(X);
				}

				Seed::Float ModD(Seed::Float In, Seed::Float & Part)
				{
					Part = static_cast<Float>(floor(In));
					return RuntimeAbs(Part - In);
				}

				Seed::Double ModD(Seed::Double In, Seed::Double & Part)
				{
					Part = floor(In);
					return RuntimeAbs(Part - In);
				}

#pragma warning(pop)
			}
		}
	}
}