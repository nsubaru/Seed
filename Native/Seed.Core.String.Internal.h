#pragma once
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Exceptions.h"

using namespace Seed;

extern __declspec(noreturn) void ThrowStringToLong(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line,
	const Seed::SizeT RequiredSize);

extern __declspec(noreturn) void ThrowNullPointException(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line,
	const Seed::SizeT Address,
	const Seed::UInt8 Mode);

extern __declspec(noreturn) void ThrowIntegerOverflowException(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line);

extern __declspec(noreturn) void ThrowInvalidFormatException(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line);