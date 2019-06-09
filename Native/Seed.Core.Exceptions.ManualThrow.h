// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Exceptions.h"

using namespace Seed;

InternalApiCall
OptimizeExport(
	extern __declspec(noreturn) void ThrowStringToLong(
		const Seed::ConstAString& File,
		const Seed::ConstAString& Function,
		const Seed::SizeT Line,
		const Seed::SizeT RequiredSize),
	{
		Throw(StringToLong(File, Function, Line, RequiredSize));
	}
);

InternalApiCall
OptimizeExport(
	extern __declspec(noreturn) void ThrowNullPointException(
		const Seed::ConstAString& File,
		const Seed::ConstAString& Function,
		const Seed::SizeT Line,
		const Seed::SizeT Address,
		const Seed::UInt8 Mode),
	{
		Throw(NullPointException(File, Function, Line, Address, static_cast<NullPointException::Types>(Mode)));
	}
);

InternalApiCall
OptimizeExport(
	extern __declspec(noreturn) void ThrowIntegerOverflowException(
		const Seed::ConstAString& File,
		const Seed::ConstAString& Function,
		const Seed::SizeT Line),
	{
		Throw(IntegerOverflowException(File, Function, Line));
	}
);

InternalApiCall
OptimizeExport(
	extern __declspec(noreturn) void ThrowInvalidFormatException(
		const Seed::ConstAString& File,
		const Seed::ConstAString& Function,
		const Seed::SizeT Line),
	{
		Throw(InvalidFormatException(File, Function, Line));
	}
);

InternalApiCall
OptimizeExport(
	__declspec(noreturn) void ThrowIndexOutOfRange(
		const Seed::ConstAString& File,
		const Seed::ConstAString& Function,
		const Seed::SizeT Line,
		const Seed::SizeT Required,
		const Seed::SizeT MaxAllowed),
	{
	Throw(IndexOutOfRange(File, Function, Line, Required, MaxAllowed));
	}
);

InternalApiCall
OptimizeExport(
	__declspec(noreturn) void ThrowEndNodeNotDeferencable(
		const Seed::ConstAString& File,
		const Seed::ConstAString& Function,
		const Seed::SizeT Line),
	{
	Throw(EndNodeNotDeferencable(File, Function, Line));
	}
);