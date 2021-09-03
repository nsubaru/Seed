// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <typeinfo.h>

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "ntapi.h"

#ifdef _CPPRTTI

size_t type_info::GetHashCode() const noexcept
{
	unsigned __int64 v2; // [rsp+0h] [rbp-28h]
	const char* i; // [rsp+8h] [rbp-20h]

	v2 = 0xCBF29CE484222325i64;
	for (i = &_Data._DecoratedName[1]; *i; ++i)
		v2 = 0x100000001B3i64 * (*(unsigned __int8*)i ^ v2);
	return (v2 >> 32) ^ v2;
}

bool type_info::operator==(const type_info& _Other) const noexcept
{
	if (&this->_Data == &_Other._Data)
		return true;
	else
		return strcmp(this->_Data._DecoratedName, _Other._Data._DecoratedName) == 0;
}

bool type_info::operator!=(const type_info& _Other) const noexcept
{
	if (&this->_Data == &_Other._Data)
		return false;
	else
		return strcmp(this->_Data._DecoratedName, _Other._Data._DecoratedName) != 0;
}

extern "C"
__declspec(noreturn)
void
__cdecl
__std_terminate() {}

#endif // _CPPRTTI