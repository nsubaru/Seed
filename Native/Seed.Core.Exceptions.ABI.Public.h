#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"

__declspec(noreturn)
void ThrowBadException();

decltype(typeid(int)) GetBadExceptionTypeId();

void SehTranslatorFunction(unsigned int exCode, struct _EXCEPTION_POINTERS* exData);