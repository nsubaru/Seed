#include "Seed.Core.Exceptions.InvalidTypeException.h"

using namespace Seed::Core;

void InvalidTypeException::ThrowMe()
{
	Throw(*this);
}

ExceptionString InvalidTypeException::toString() const
{
	Unsafe( //-V612
		return ExceptionString::Combine(
			exceptionName,
			L". The expected type: ", String(const_cast<AChar*>(RequiredType.GetCxxName())),
			L". The actual type: ", String(const_cast<AChar*>(ActuallyType.GetCxxName())),
			L". ",
			location.toString(),
			(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
		));
}