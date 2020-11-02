// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Exceptions.ABI.h"
#include "Seed.Core.Exceptions.ABI.Internal.API.h"
#include "Seed.Core.Exceptions.ABI.Internal.Types.h"
#include "Seed.Core.Exceptions.ABI.Internal.Tls.h"
#include "Seed.Core.Exceptions.ABI.Internal.Inlines.h"

EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler4(
	EHExceptionRecord* pExcept, 
	EHRegistrationNode RN, 
	CONTEXT* pContext, 
	DispatcherContext* pDC
) {
	FuncInfo4          FuncInfo;
	EXCEPTION_DISPOSITION   result{};
	EHRegistrationNode      EstablisherFrame = RN;
	GetThreadExceptionData()->_ImageBase = pDC->ImageBase;
	GetThreadExceptionData()->_ThrowImageBase = (uintptr_t)pExcept->params.pThrowImageBase;

	PBYTE buffer = (PBYTE)(pDC->ImageBase + *(PULONG)pDC->HandlerData);

	DecompFuncInfo(buffer, FuncInfo, pDC->ImageBase, pDC->FunctionEntry->BeginAddress);

	result = InternalCxxFrameHandler(pExcept, &EstablisherFrame, pContext, pDC, &FuncInfo, 0, nullptr, FALSE);
	return result;
}
