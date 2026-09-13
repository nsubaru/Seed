#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.TypeTraits.h"
#include "Seed.Core.Base.Concepts.h"

namespace Seed::Core::TypeTraits
{
	/// <summary>
    /// Invoke static assert with a message which contain if the condition is false.
	/// </summary>
	/// <typeparam name="TargetType">Additional information type</typeparam>
	/// <typeparam name="T1">Left operand type</typeparam>
	/// <typeparam name="T2">Right operand type</typeparam>
	/// <param name="t1">Left operand value</param>
	/// <param name="t2">Right operand value</param>
    template<typename TargetType, auto t1, auto t2> requires requires() {
        {
            t1 != t2
        } -> Concepts::IsSame<bool>;
	}
    struct SmartStaticAssert
    {
	private:
        static constexpr bool Check() {
            if constexpr (t1 == t2)
            {
                static_assert(t1 != t2, "SmartStaticAssert failure. Condition: t1 != t2. Details: " __FUNCSIG__);
                return false;
            }
			else
			{
				return true;
			}
        }

	public:
		/// <summary>
		/// Compile-time result of the configured static assertion check.
		/// </summary>
		static constexpr auto Result = Check();
    };
}
