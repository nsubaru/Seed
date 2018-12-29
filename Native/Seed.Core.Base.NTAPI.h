#pragma once
#ifndef SEED_RT_ALLOW_INTERNAL_NT_API
#error "Internal NT API not allowed"
#endif // !SEED_RT_ALLOW_INTERNAL_NT_API

#pragma comment(lib, "ntdll.lib")

#include "ntapi.h"

#define _RTTI_RELATIVE_TYPEINFO 1