#ifdef ProfilingMode
#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#define DECLSPEC_IMPORT __declspec(dllimport)
#define WINBASEAPI DECLSPEC_IMPORT
#define WINAPI      __stdcall

extern "C"
WINBASEAPI
BOOL
WINAPI
AllocConsole(
    VOID
);

extern "C"
WINBASEAPI
BOOL
WINAPI
FreeConsole(
    VOID
);

void EnableWinApiSupport()
{
    // Create console if there aren't any attached console
    AllocConsole();
}
#endif // ProfilingMode