#include "Seed.Core.Services.Dns.Exceptions.Base.h"

using namespace Seed::Core;

void Seed::Core::Services::Dns::DnsException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::Services::Dns::DnsException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		location.toString(),
		L" ",
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}