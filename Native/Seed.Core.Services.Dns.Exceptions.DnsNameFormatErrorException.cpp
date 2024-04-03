#include "Seed.Core.Services.Dns.Exceptions.DnsNameFormatErrorException.h"

void Seed::Core::Services::Dns::DnsNameFormatErrorException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
