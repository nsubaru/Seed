#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.StaticAsserts.h"

namespace Seed {
	inline namespace Core {
		namespace Concepts {
			template<typename T>
			concept Comparable = requires(T a, T b) {
				{a == b}->Boolean&&
				{a != b}->Boolean&&
				{a >= b}->Boolean&&
				{a <= b}->Boolean&&
				{a < b}->Boolean&&
				{a > b}->Boolean&&
			};
		}
	}
}