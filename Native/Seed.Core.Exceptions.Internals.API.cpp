// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Exceptions.h"

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowNullPointException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT Addr, UInt8 Type) {
	Throw(NullPointException(File, FunctName, Line, Addr, static_cast<NullPointException::Types>(Type)));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowStringToLong(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line, const Seed::SizeT RequiredSize) {
	Throw(StringToLong(File, Function, Line, RequiredSize));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowNullPointException(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(NullPointException(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowIntegerOverflowException(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(IntegerOverflowException(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowInvalidFormatException(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(InvalidFormatException(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowIndexOutOfRange(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line, const Seed::SizeT Required, const Seed::SizeT MaxAllowed) {
	Throw(IndexOutOfRange(File, Function, Line, Required, MaxAllowed));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowEndNodeNotDeferencable(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(EndNodeNotDeferencable(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowNotImplemented(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(NotImplementedException(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowKeyAlredyExist(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(KeyAllredyExistException(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowNotFound(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(NotFoundException(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowInternalError(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(InternalError(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowDivideByZero(const Seed::ConstAString& File, const Seed::ConstAString& Function, const Seed::SizeT Line) {
	Throw(DivideByZero(File, Function, Line));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::ThrowInvalidArgumentException(const Seed::ConstAString& File, const Seed::ConstAString& Function, Seed::SizeT Line, const Seed::ConstString& ArgName, const Seed::ConstString& Description) {
	Throw(InvalidArgumentException(File, Function, Line, ArgName, Description));
}

__declspec(noreturn) void Seed::Core::Internals::Exceptions::__ReThrow() {
	ReThrow;
}

