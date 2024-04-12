#include "Seed.Core.Services.Dns.Exceptions.DnsNotImplementedException.h"

void Seed::Core::Services::Dns::DnsNotImplementedException::ThrowMe()
{
	Throw(*this);
}
