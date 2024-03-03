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
		/// Indicates whether the variable is a space
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if space</returns>
		static bool IsSpace(Char ch) {
			return ch == L' ' ||
				ch == 0xa0 ||
				ch == 0x2002 ||
				ch == 0x2003 ||
				ch == 0x2004 ||
				ch == 0x2005 ||
				ch == 0x2006 ||
				ch == 0x2007 ||
				ch == 0x2008 ||
				ch == 0x2009 ||
				ch == 0x200A ||
				ch == 0x200B ||
				ch == 0x202F;
		}

		/// <summary>
		/// Indicates whether the variable is a latin letter
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if latin letter</returns>
		static bool IsLatinLetter(Char ch)
		{
			return IsUpperLatinLetter(ch) || IsLowerLatinLetter(ch);
		}

		/// <summary>
		/// Indicates whether the variable is a uppercase latin letter
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if uppercase latin letter</returns>
		static bool IsUpperLatinLetter(Char ch)
		{
			return (L'A' <= ch && ch <= L'Z') ;
		}

		/// <summary>
		/// Indicates whether the variable is a lowercase latin letter
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if lowercase latin letter</returns>
		static bool IsLowerLatinLetter(Char ch)
		{
			return (L'a' <= ch && ch <= L'z');
		}

#ifdef SeedKernel
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
#endif // __has_include('ntapi.h')
	};
}