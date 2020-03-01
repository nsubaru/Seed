#pragma once
#include "Seed.Core.Crypt.Base.h"

namespace Seed::Core::Crypt
{
	/// <summary>
	/// Вертає випадкове число в діапазоні 0 - (Max UInt32 - 1)
	/// </summary>
	UInt32 Random();
}