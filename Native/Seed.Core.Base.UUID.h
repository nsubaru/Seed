#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Math.h"
#include "Seed.Core.Base.Comparing.h"

#ifndef __GUID_DEFINED
#define __GUID_DEFINED
typedef struct _GUID {
	unsigned long  Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
} GUID;
#endif // __GUID_DEFINED

#ifndef SeedKernelCall
#ifdef SeedKernel
#define SeedKernelCall __declspec(dllexport)
#else
#define SeedKernelCall __declspec(dllimport)
#endif
#endif

namespace Seed {
	inline namespace Core {		
		/// <summary>
		/// Клас унікального ідентифікатора UUID сумісного з GUID який використовується в ядрі Windows
		/// </summary>
		class UUID {
		private:
			/// <summary>
			/// значення об'єкту
			/// </summary>
			GUID uuid;
		public:
			/// <summary>
			/// Маска UUID для форматування рядка
			/// </summary>
			static constexpr Char UUIDStringWildCard[38] = L"{????????-????-????-????????????????}";

			/// <summary>
			/// Нульовий UUID
			/// </summary>
			/// <returns>Вертає нульовий UUID</returns>
			constexpr __forceinline static UUID GetZeroUUID() { return UUID(); };

			/// <summary>
			/// Максимальний можливий UUID
			/// </summary>
			/// <returns>Вертає максимальний можливий UUID</returns>
			constexpr __forceinline static UUID GetMaxUUID() { return UUID(GUID({ 0xffffffff,0xffff,0xffff,{0xff,0xff,0xff,0xff,0xff} })); };//-V112 //-V2523

			/// <summary>
			/// Пустий конструктор, генерує нульовий ідентифікатор
			/// </summary>
			/// <returns>Нульовий ідентифікатор</returns>
			constexpr __forceinline UUID() : uuid({ 0,0,0, {0,0,0,0,0,0,0,0}}) { }

			/// <summary>
			/// Конструктор з GUID
			/// </summary>
			/// <param name="uuid">GUID значення</param>
			/// <returns>Ідентифікатор</returns>
			constexpr __forceinline UUID(const GUID& uuid) : uuid(uuid) {}

			/// <summary>
			/// Неявне перетворення в GUID потрібно для сумісності з Nt і Rtl функціями ntdll
			/// </summary>
			/// <returns>GUID з значенням об'єкта</returns>
			constexpr __forceinline operator const GUID& () const { return this->uuid; }

			/// <summary>
			/// Постфіксний декремент
			/// </summary>
			/// <returns></returns>
			constexpr __forceinline UUID& operator--() {
				if (uuid.Data4[7] == 0)
					if (uuid.Data4[6] == 0)
						if (uuid.Data4[5] == 0)
							if (uuid.Data4[4] == 0)
								if (uuid.Data4[3] == 0)
									if (uuid.Data4[2] == 0)
										if (uuid.Data4[1] == 0)
											if (uuid.Data4[0] == 0)
												if (uuid.Data3 == 0)
													if (uuid.Data2 == 0)
														if (uuid.Data1 != 0) uuid.Data1 -= 1;
														else return *this;
													else uuid.Data2 -= 1;
												else uuid.Data3 -= 1;
											else uuid.Data4[0] -= 1;
										else uuid.Data4[1] -= 1;
									else uuid.Data4[2] -= 1;
								else uuid.Data4[3] -= 1;
							else uuid.Data4[4] -= 1;
						else uuid.Data4[5] -= 1;
					else uuid.Data4[6] -= 1;
				else uuid.Data4[7] -= 1;
				return *this;
			}
			
			/// <summary>
			/// Постфіксний інкремент
			/// </summary>
			/// <returns>Нове значення</returns>
			constexpr __forceinline UUID& operator++() {
				if (uuid.Data4[7] == Seed::Math::Max<UInt8>())
					if (uuid.Data4[6] == Seed::Math::Max<UInt8>())
						if (uuid.Data4[5] == Seed::Math::Max<UInt8>())
							if (uuid.Data4[4] == Seed::Math::Max<UInt8>())
								if (uuid.Data4[3] == Seed::Math::Max<UInt8>())
									if (uuid.Data4[2] == Seed::Math::Max<UInt8>())
										if (uuid.Data4[1] == Seed::Math::Max<UInt8>())
											if (uuid.Data4[0] == Seed::Math::Max<UInt8>())
												if (uuid.Data3 == Seed::Math::Max<UInt16>())
													if (uuid.Data2 == Seed::Math::Max<UInt16>())
														if (uuid.Data1 != Seed::Math::Max<UInt32>()) uuid.Data1 += 1;
														else return *this;
													else uuid.Data2 += 1;
												else uuid.Data3 += 1;
											else uuid.Data4[0] += 1;
										else uuid.Data4[1] += 1;
									else uuid.Data4[2] += 1;
								else uuid.Data4[3] += 1;
							else uuid.Data4[4] += 1;
						else uuid.Data4[5] += 1;
					else uuid.Data4[6] += 1;
				else uuid.Data4[7] += 1;
				return *this;
			}

			/// <summary>
			/// Префіксний декремент
			/// </summary>
			/// <returns>Вертає старе значення</returns>
			constexpr __forceinline UUID operator--(int) {
				const UUID uuidCp = this->uuid;
				--*this;
				return uuidCp;
			}
			
			/// <summary>
			/// Префіксний інкремент
			/// </summary>
			/// <returns>Вертає старе значення</returns>
			constexpr __forceinline UUID operator++(int) {
				const UUID uuidCp = this->uuid;
				++*this;
				return uuidCp;
			}

			/// <summary>
			/// Універсальне порівняння
			/// </summary>
			/// <returns>Відносність об'єктів</returns>
			StrongOrdering operator<=>(const UUID&) const = default;
		};
	}
}