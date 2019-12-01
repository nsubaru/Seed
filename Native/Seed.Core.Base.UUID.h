#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Math.h"

#ifndef __GUID_DEFINED
#define __GUID_DEFINED
///<summary>128-бітний унікальний ідентифікатор</summary>
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
		class SeedKernelCall UUID {
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
			/// Перевірка на рівність
			/// </summary>
			/// <param name="a">Праве значення</param>
			/// <param name="b">Ліве значення</param>
			/// <returns>True якщо рівні, False якщо ні</returns>
			constexpr __forceinline friend Boolean operator==(const UUID& a, const UUID& b) {
				if (a.uuid.Data4[7] == b.uuid.Data4[7])
					if (a.uuid.Data4[6] == b.uuid.Data4[6])
						if (a.uuid.Data4[5] == b.uuid.Data4[5])
							if (a.uuid.Data4[4] == b.uuid.Data4[4])
								if (a.uuid.Data4[3] == b.uuid.Data4[3])
									if (a.uuid.Data4[2] == b.uuid.Data4[2])
										if (a.uuid.Data4[1] == b.uuid.Data4[1])
											if (a.uuid.Data4[0] == b.uuid.Data4[0])
												if (a.uuid.Data3 == b.uuid.Data3)
													if (a.uuid.Data2 == b.uuid.Data2)
														if (a.uuid.Data1 == b.uuid.Data1) return true;
				return false;
			}

			/// <summary>
			/// Перевірка на нерівність
			/// </summary>
			/// <param name="a">Праве значення</param>
			/// <param name="b">Ліве значення</param>
			/// <returns>True якщо не рівні, інакше False</returns>
			constexpr __forceinline friend Boolean operator!=(const UUID& a, const UUID& b) {
				return !(a == b);
			}

			/// <summary>
			/// Перевірка a менше b
			/// </summary>
			/// <param name = "a">Праве значення</param>
			/// <param name = "b">Ліве значення</param>
			/// <returns>True якщо праве менше ніж ліве, False інакше</returns>
			constexpr __forceinline friend Boolean operator<(const UUID& a, const UUID& b) {
				if (a.uuid.Data4[7] < b.uuid.Data4[7])
					if (a.uuid.Data4[6] < b.uuid.Data4[6])
						if (a.uuid.Data4[5] < b.uuid.Data4[5])
							if (a.uuid.Data4[4] < b.uuid.Data4[4])
								if (a.uuid.Data4[3] < b.uuid.Data4[3])
									if (a.uuid.Data4[2] < b.uuid.Data4[2])
										if (a.uuid.Data4[1] < b.uuid.Data4[1])
											if (a.uuid.Data4[0] < b.uuid.Data4[0])
												if (a.uuid.Data3 < b.uuid.Data3)
													if (a.uuid.Data2 < b.uuid.Data2)
														if (a.uuid.Data1 < b.uuid.Data1) return true;
				return false;
			}
			
			/// <summary>
			/// Перевірка a більше b
			/// </summary>
			/// <param name="a">Праве значення</param>
			/// <param name="b">Ліве значення</param>
			/// <returns>True якщо праве більше ніж ліве, False інакше</returns>
			constexpr __forceinline friend Boolean operator>(const UUID& a, const UUID& b) {
				if (a.uuid.Data4[7] > b.uuid.Data4[7])
					if (a.uuid.Data4[6] > b.uuid.Data4[6])
						if (a.uuid.Data4[5] > b.uuid.Data4[5])
							if (a.uuid.Data4[4] > b.uuid.Data4[4])
								if (a.uuid.Data4[3] > b.uuid.Data4[3])
									if (a.uuid.Data4[2] > b.uuid.Data4[2])
										if (a.uuid.Data4[1] > b.uuid.Data4[1])
											if (a.uuid.Data4[0] > b.uuid.Data4[0])
												if (a.uuid.Data3 > b.uuid.Data3)
													if (a.uuid.Data2 > b.uuid.Data2)
														if (a.uuid.Data1 > b.uuid.Data1) return true;
				return false;
			}
			
			/// <summary>
			/// Перевірка a більше рівне b
			/// </summary>
			/// <param name="a">Праве значення</param>
			/// <param name="b">Ліве значення</param>
			/// <returns>True якщо праве менша або рівне ніж ліве, False інакше</returns>
			constexpr __forceinline friend Boolean operator>=(const UUID& a, const UUID& b) { return a == b || a > b; }
			
			/// <summary>
			/// Перевірка a меншу рівне b
			/// </summary>
			/// <param name="a">Праве значення</param>
			/// <param name="b">Ліве значення</param>
			/// <returns>True якщо праве більше або рівне ніж ліве, False інакше</returns>
			constexpr __forceinline friend Boolean operator<=(const UUID& a, const UUID& b) { return a == b || a < b; }
		};
	}
}