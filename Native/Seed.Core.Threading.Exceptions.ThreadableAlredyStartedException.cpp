#include "Seed.Core.Threading.Exceptions.ThreadableAlredyStartedException.h"

using namespace Seed::Core;

void Seed::Core::Threading::ThreadableAlredyStartedException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::Threading::ThreadableAlredyStartedException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		Location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}