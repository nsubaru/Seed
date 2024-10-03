// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.Services.Dns.Exceptions.DnsServerFailureException.h"

void Seed::Core::Services::Dns::DnsServerFailureException::ThrowMe()
{
	Throw(*this);
}
