#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Comparing.h"

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

		/// <summary>
		/// Case insensitive character comparing
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>Comparing result</returns>
		static StrongOrdering CaseInsensitiveCompare(AChar left, AChar right);

		/// <summary>
		/// Case insensitive character comparing
		/// </summary>
		/// <param name="left">Left operand</param>
		/// <param name="right">Right operand</param>
		/// <returns>Comparing result</returns>
		static StrongOrdering CaseInsensitiveCompare(UChar left, UChar right);
	};
}