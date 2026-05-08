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
		/// Indicates whether the variable is a  hex digit
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if hex digit</returns>
		static bool IsHexDigit(Char ch)
		{
			return IsDigit(ch) || (L'a' <= ch && ch <= L'f') || (L'A' <= ch && ch <= L'F');
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

		/// <summary>
		/// Indicates whether the variable is a latin letter or digit
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if latin letter or digit</returns>
		static bool IsAlphaNumeric(Char ch)
		{
			return IsDigit(ch) || IsLatinLetter(ch);
		}

		/// <summary>
		/// Indicates whether the variable is a white space (includes spaces, tabs, newlines, etc.)
		/// </summary>
		/// <param name="ch">Given character</param>
		/// <returns>"True" if white space</returns>
		static bool IsWhiteSpace(Char ch) {
			return ch == L' '   ||  // Space
				   ch == L'\t'  ||  // Horizontal tab
				   ch == L'\n'  ||  // Line feed
				   ch == L'\v'  ||  // Vertical tab
				   ch == L'\f'  ||  // Form feed
				   ch == L'\r'  ||  // Carriage return
				   ch == 0x85   ||  // NEXT LINE (NEL)
				   ch == 0xa0   ||  // NO-BREAK SPACE
				   ch == 0x1680 ||  // OGHAM SPACE MARK
				   (ch >= 0x2000 && ch <= 0x200B) ||  // EN QUAD to ZERO WIDTH SPACE
				   ch == 0x2028 ||  // LINE SEPARATOR
				   ch == 0x2029 ||  // PARAGRAPH SEPARATOR
				   ch == 0x202F ||  // NARROW NO-BREAK SPACE
				   ch == 0x205F ||  // MEDIUM MATHEMATICAL SPACE
				   ch == 0x3000;    // IDEOGRAPHIC SPACE
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