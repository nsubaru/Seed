#include "Seed.Core.Services.Dns.Exceptions.DnsActionRefusedException.h"

void Seed::Core::Services::Dns::DnsActionRefusedException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
