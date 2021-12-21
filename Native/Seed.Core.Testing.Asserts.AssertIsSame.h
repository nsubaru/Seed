#pragma once
#include "Seed.Core.Testing.Asserts.Base.h"
#include "Seed.Core.Testing.Asserts.Exceptions.h"

namespace Seed::Core::Details
{
	class AssertIsSamePart {
	public:
		AssertIsSamePart() = delete;
		AssertIsSamePart(const AssertIsSamePart&) = delete;
		AssertIsSamePart(AssertIsSamePart&&) = delete;

	public:
		/// <summary>
		/// Checking is two objects are same using method IsSameAs
		/// </summary>
		/// <typeparam name="T">Variable type</typeparam>
		/// <param name="a">First variable value</param>
		/// <param name="b">Second variable value</param>
		/// <param name="sourceLocation">Verification call location</param>
		template<typename T> requires requires(T a, T b) { {a.IsSameAs(b)} -> Concepts::IsSame<bool>; }
		ForceInline static void AssertIsSame(T&& a, T&& b, const SourceLocation sourceLocation) {
			if (!a.IsSameAs(b))
				Throw(AssertSameException(sourceLocation));
		}

		/// <summary>
		/// Checking is two objects are same using method IsSameAs
		/// </summary>
		/// <typeparam name="T">Variable type</typeparam>
		/// <param name="a">First variable value</param>
		/// <param name="b">Second variable value</param>
		/// <param name="sourceLocation">Verification call location</param>
		template<typename T> requires requires(T a, T b) { {a.IsSameAs(b)} -> Concepts::IsSame<bool>; }
		ForceInline static void AssertIsSame(const T& a, T&& b, SourceLocation sourceLocation) {
			if (!a.IsSameAs(b))
				Throw(AssertSameException(sourceLocation));
		}

		/// <summary>
		/// Checking is two objects are same using method IsSameAs
		/// </summary>
		/// <typeparam name="T">Variable type</typeparam>
		/// <param name="a">First variable value</param>
		/// <param name="b">Second variable value</param>
		/// <param name="sourceLocation">Verification call location</param>
		template<typename T> requires requires(T a, T b) { {a.IsSameAs(b)} -> Concepts::IsSame<bool>; }
		ForceInline static void AssertIsSame(T&& a, const T& b, SourceLocation sourceLocation) {
			if (!a.IsSameAs(b))
				Throw(AssertSameException(sourceLocation));
		}

		/// <summary>
		/// Checking is two objects are same using method IsSameAs
		/// </summary>
		/// <typeparam name="T">Variable type</typeparam>
		/// <param name="a">First variable value</param>
		/// <param name="b">Second variable value</param>
		/// <param name="sourceLocation">Verification call location</param>
		template<typename T> requires requires(T a, T b) { {a.IsSameAs(b)} -> Concepts::IsSame<bool>; }
		ForceInline static void AssertIsSame(const T& a, const T& b, SourceLocation sourceLocation) {
			if (!a.IsSameAs(b))
				Throw(AssertSameException(sourceLocation));
		}
	};
}