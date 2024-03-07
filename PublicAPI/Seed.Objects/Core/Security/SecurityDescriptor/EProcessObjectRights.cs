using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Security.SecurityDescriptor;

[SerializableEnum("Seed::Core::Security::EProcessObjectRights")]
public enum EProcessObjectRights : UInt64
{
    ProcessTerminate = 1,
		ProcessCreateThread = 2,
		ProcessSetSessionId = 3,
		ProcessVmOperation = 4,
		ProcessVmRead = 5,
		ProcessVmWrite = 6,
		ProcessDupHandle = 7,
		ProcessCreateProcess = 8,
		ProcessSetQuota = 9,
		ProcessSetInformation = 10,
		ProcessQueryInformation = 11,
		ProcessSuspendResume = 12,
		ProcessQueryLimitedInformation = 13,
		ProcessSetLimitedInformation = 14,
		ProcessReserved1 = 15,
		ProcessReserved2 = 16,
		Delete = 17,
		ReadControl = 18,
		WriteDac = 19,
		WriteOwner = 20,
		Synchronize = 21
};
