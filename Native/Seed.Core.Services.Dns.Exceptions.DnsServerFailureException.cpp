#include "Seed.Core.Services.Dns.Exceptions.DnsServerFailureException.h"

void Seed::Core::Services::Dns::DnsServerFailureException::ThrowMe()
{
	Throw(*this);
}
