#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		///<summary>Виставлення 1 в байті b під індексом bitIndex. Порядок bigEndian</summary>
		///<param name="b"> Байт в якому треба виставити біт</param>
		///<param name="bitIndex"> Номер біта від нуля</param>
		constexpr __forceinline void BitSet(Byte& b, SizeT bitIndex) {
			b |= 1 << bitIndex;
		}

		///<summary>Виставлення 0 в байті b під індексом bitIndex. Порядок bigEndian</summary>
		///<param name="b"> Байт в якому треба виставити біт</param>
		///<param name="bitIndex"> Номер біта від нуля</param>
		constexpr __forceinline void BitReset(Byte& b, SizeT bitIndex) {
			b &= ~(1 << bitIndex);
		}

		///<summary>Провірка біта під індексом bitIndex в байті b. Порядок bigEndian</summary>
		///<param name="b"> Байт в якому треба виставити біт</param>
		///<param name="bitIndex"> Номер біта від нуля</param>
		///<returns>True якщо 1, якщо 0 False</returns>
		constexpr __forceinline Boolean BitCheck(const Byte b, SizeT bitIndex) { //-V669
			return (b & (1 << bitIndex)) == 1;
		}
	}
}