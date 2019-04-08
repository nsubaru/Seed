#include "Seed.Core.String.Internal.h"

__declspec(noreturn) void ThrowStringToLong(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line,
	const Seed::SizeT RequiredSize)
{
	Throw(StringToLong(File, Function, Line, RequiredSize));
}

__declspec(noreturn) void ThrowNullPointException(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line,
	const Seed::SizeT Address,
	const Seed::UInt8 Mode)
{
	Throw(NullPointException(File, Function, Line, Address, static_cast<NullPointException::Types>(Mode)));
}

__declspec(noreturn) void ThrowIntegerOverflowException(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line
)
{
	Throw(IntegerOverflowException(File, Function, Line));
}

__declspec(noreturn) void ThrowInvalidFormatException(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line
)
{
	Throw(InvalidFormatException(File, Function, Line));
}