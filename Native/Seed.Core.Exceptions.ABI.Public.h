#pragma once
#include "Seed.Core.ConstString.h"

#include "Seed.Core.Exceptions.ABI.Base.h"


//Implement in public Exception subsystem API use in private ABI
__declspec(noreturn)
void ThrowBadException();

decltype(typeid(int)) GetBadExceptionTypeId();

void SehTranslatorFunction(unsigned int exCode, struct _EXCEPTION_POINTERS* exData);

__declspec(noreturn)
void EmergencyShutdown(
    Seed::Core::ConstAString File,
    Seed::Core::ConstAString Function,
    Seed::Core::UInt64 Line,
    Seed::Core::ConstString Reason
);

//Implement in Exception private ABI and use in API 
void StoreException(void*& ptr, void* (*alocator)(Seed::SizeT));

void ThrowStoredException(void* ptr);

void DestroyStoredException(void*& ptr, void(*dealocator)(void*));