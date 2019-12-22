#pragma once
#include "Seed.Core.Base.Types.h"

namespace Seed {
	inline namespace Core {
		/// <summary>
		/// Бітові операції
		/// </summary>
		class Bits {
		public:
			/// <summary>
			/// Виставлення 1 в байті b під індексом bitIndex. Порядок bigEndian
			/// </summary>
			/// <param name="b">Байт в якому треба виставити біт</param>
			/// <param name="bitIndex">Номер біта від нуля</param>
			template<typename TInt = Byte>
			static constexpr __forceinline void BitSet(TInt& b, const SizeT bitIndex) {
				b |= 1 << bitIndex;
			}

			/// <summary>
			/// Виставлення 0 в байті b під індексом bitIndex. Порядок bigEndian
			/// </summary>
			/// <param name="b">Байт в якому треба виставити біт</param>
			/// <param name="bitIndex">Номер біта від нуля</param>
			template<typename TInt = Byte>
			static constexpr __forceinline void BitReset(TInt& b, const SizeT bitIndex) {
				b &= ~(1 << bitIndex);
			}

			/// <summary>
			/// Перевірка біта під індексом bitIndex в байті b. Порядок bigEndian
			/// </summary>
			/// <param name="b">Байт в якому треба виставити біт</param>
			/// <param name="bitIndex">Номер біта від нуля</param>
			/// <returns>True якщо 1, якщо 0 False</returns>
			template<typename TInt = Byte>
			static constexpr __forceinline Boolean BitCheck(const TInt& b, const SizeT bitIndex) { //-V669
				return b & (1 << bitIndex);
			}

			/// <summary>
			/// Витягуєм count бітів з позиції position і вертає їх результат
			/// </summary>
			/// <param name="value">Значення</param>
			/// <param name="position">Позиція першого біту</param>
			/// <param name="count">Кількість бітів</param>
			template<typename TInt = Byte>
			TInt ExtractBitsFromInt(const TInt& value, const TInt& position, const TInt& count) {
				TInt mask = (1 << count) - 1;
				return ((value >> position) & mask) << position;
			}

			/// <summary>
			/// Інвертує біт по позиції position і веритає нове значення
			/// </summary>
			/// <param name="value">Значення до інвертування</param>
			/// <param name="postion">Номер біта який інвертує</param>
			template<typename TInt = Byte>
			TInt FlipBit(const TInt& value, const TInt& position) {
				return value ^= 1 << position;
			}

			Bits() = delete;
			Bits(const Bits&) = delete;
			Bits(Bits&&) = delete;
			Bits& operator=(const Bits&) = delete;
			Bits& operator=(Bits&&) = delete;
		};
	}
}