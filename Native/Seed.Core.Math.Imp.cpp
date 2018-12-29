#include "Seed.Core.Math.Imp.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

using namespace Seed;
#pragma warning(push)
#pragma warning(disable: 4162)
#pragma intrinsic(pow)
Double Seed::Core::Details::RuntimePow(Double What, Double By)
{
	return pow(What, By);
}

#pragma intrinsic(pow)
Float Seed::Core::Details::RuntimePow(Float What, Float By)
{
	return (Float)pow(What, By);
}

UInt64 Seed::Core::Details::RuntimePow(UInt64 What, UInt8 By)
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

	switch (highest_bit_set[By]) {
	case 255: //ми можем використовувати 255 як маркер переповнення
		if (What == 1) {
			return 1;
		}

		if (What == -1) {
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

Double exp(Double X) {
	return 1.0 + X +
		Details::RuntimePow(X, 2) / Details::ConstexprFactorial(2) +
		Details::RuntimePow(X, 3) / Details::ConstexprFactorial(3) +
		Details::RuntimePow(X, 4) / Details::ConstexprFactorial(4) +
		Details::RuntimePow(X, 5) / Details::ConstexprFactorial(5) +
		Details::RuntimePow(X, 6) / Details::ConstexprFactorial(6) +
		Details::RuntimePow(X, 7) / Details::ConstexprFactorial(7) +
		Details::RuntimePow(X, 8) / Details::ConstexprFactorial(8) +
		Details::RuntimePow(X, 9) / Details::ConstexprFactorial(9);
}

#pragma intrinsic(exp)
Double Details::RuntimeExp(Double X) {
	return exp(X);
}

#pragma intrinsic(sin)
Double Seed::Core::Details::RuntimeSin(Double X)
{
	return sin(X);
}

#pragma intrinsic(cos)
Double Seed::Core::Details::RuntimeCos(Double X)
{
	return cos(X);
}

#pragma intrinsic(tan)
Double Seed::Core::Details::RuntimeTan(Double X)
{
	return tan(X);
}

#pragma intrinsic(cos)
#pragma intrinsic(sin)
Double Seed::Core::Details::RuntimeCtan(Double X)
{
	return sin(X) / cos(X);
}

#pragma intrinsic(asin)
Double Seed::Core::Details::RuntimeASin(Double X)
{
	return asin(X);
}

Double Seed::Core::Details::RuntimeACos(Double X)
{
	return (Seed::Core::Details::Pi / 2) -  asin(X);
}

#pragma intrinsic(atan)
Double Seed::Core::Details::RuntimeATan(Double X)
{
	return atan(X);
}

#pragma intrinsic(atan)
Double Seed::Core::Details::RuntimeACtan(Double X)
{
	return (Seed::Core::Details::Pi / 2) - atan(X);
}

#pragma intrinsic(sqrt)
Double Seed::Core::Details::RuntimeSqrt(Double X)
{
	return sqrt(X);
}

#pragma warning(pop)