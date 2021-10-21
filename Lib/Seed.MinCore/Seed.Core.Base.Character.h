#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed::Core
{
	/// <summary>
	/// General functions for working with characters
	/// </summary>
	class Characters
	{
	public:
		/// <summary>
		/// Indicates whether the variable is a digit
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if digit</returns>
		static bool IsDigit(Char ch)
		{
			return L'0' <= ch && ch <= L'9';
		}
	};
}