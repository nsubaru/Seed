#pragma once
#include "Seed.Core.Base.h"

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
		//Клас унікального ідентифікатора UUID сумісного з GUID
		// який використовується в ядрі Windows
		class SeedKernelCall UUID {
		private:
			//GUID для зберігання даних
			GUID uuid;
		public:
			static constexpr Char UUIDStringWildCard[38] = L"{????????-????-????-????????????????}";

			//Вертає нульовий UUID
			constexpr __forceinline static UUID GetZeroUUID() noexcept { return UUID(); }

			//Вертає максимальний можливий UUID
			__forceinline static UUID GetMaxUUID() noexcept {
				return GUID({ 0xffffffff,0xffff,0xffff,{0xff,0xff,0xff,0xff,0xff} }); //-V112 //-V2523
			}

			//Пустий конструктор, генерує нульовий ідентифікатор
			constexpr __forceinline UUID() noexcept : uuid({ 0 }) { }
			//Конструктор з GUID
			__forceinline UUID(const GUID& uuid) noexcept { this->uuid = uuid; }

			//Неявне перетворення в GUID потрібно для сумісності з Nt і Rtl функціями ntdll
			__forceinline operator const GUID& () const noexcept { return this->uuid; }

			//Постфіксний декремени
			__forceinline UUID& operator--() noexcept {
				if (uuid.Data4[3] == 0)
					if (uuid.Data4[2] == 0)
						if (uuid.Data4[1] == 0)
							if (uuid.Data4[0] == 0)
								if (uuid.Data3 == 0)
									if (uuid.Data2 == 0)
										if (!uuid.Data1 == 0) uuid.Data1 -= 1;
										else return *this;
									else uuid.Data2 -= 1;
								else uuid.Data3 -= 1;
							else uuid.Data4[0] -= 1;
						else uuid.Data4[1] -= 1;
					else uuid.Data4[2] -= 1;
				else uuid.Data4[3] -= 1;
				return *this;
			}
			//Постфіксний інкремент
			__forceinline UUID & operator++() noexcept {
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
				return *this;
			}

			//Префіксний декремент
			__forceinline UUID operator--(int) noexcept {
				GUID uuidCp = this->uuid;
				if (uuidCp.Data4[3] == 0)
					if (uuidCp.Data4[2] == 0)
						if (uuidCp.Data4[1] == 0)
							if (uuidCp.Data4[0] == 0)
								if (uuidCp.Data3 == 0)
									if (uuidCp.Data2 == 0)
										if (!uuidCp.Data1 == 0) uuidCp.Data1 -= 1;
										else return uuidCp;
									else uuidCp.Data2 -= 1;
								else uuidCp.Data3 -= 1;
							else uuidCp.Data4[0] -= 1;
						else uuidCp.Data4[1] -= 1;
					else uuidCp.Data4[2] -= 1;
				else uuidCp.Data4[3] -= 1;
				return uuidCp;
			}
			//Префіксний інкремент
			__forceinline UUID operator++(int) noexcept {
				GUID uuidCp = this->uuid;
				if (uuidCp.Data4[3] == Seed::Math::Max<UInt8>())
					if (uuidCp.Data4[2] == Seed::Math::Max<UInt8>())
						if (uuidCp.Data4[1] == Seed::Math::Max<UInt8>())
							if (uuidCp.Data4[0] == Seed::Math::Max<UInt8>())
								if (uuidCp.Data3 == Seed::Math::Max<UInt16>())
									if (uuidCp.Data2 == Seed::Math::Max<UInt16>())
										if (uuidCp.Data1 != Seed::Math::Max<UInt32>()) uuidCp.Data1 += 1;
										else return uuidCp;
									else uuidCp.Data2 += 1;
								else uuidCp.Data3 += 1;
							else uuidCp.Data4[0] += 1;
						else uuidCp.Data4[1] += 1;
					else uuidCp.Data4[2] += 1;
				else uuidCp.Data4[3] += 1;
				return uuidCp;
			}

			//Провірка на рівність
			__forceinline friend Boolean operator==(const UUID & a, const UUID & b) noexcept {
				if (a.uuid.Data4[3] == b.uuid.Data4[3])
					if (a.uuid.Data4[2] == b.uuid.Data4[2])
						if (a.uuid.Data4[1] == b.uuid.Data4[1])
							if (a.uuid.Data4[0] == b.uuid.Data4[0])
								if (a.uuid.Data3 == b.uuid.Data3)
									if (a.uuid.Data2 == b.uuid.Data2)
										if (a.uuid.Data1 == b.uuid.Data1)
											return true;
				return false;
			}

			//Провірка на нерівність
			__forceinline friend Boolean operator!=(const UUID & a, const UUID & b) noexcept {
				return !(a == b);
			}
			//Провірка a < b
			__forceinline friend Boolean operator<(const UUID & a, const UUID & b) noexcept {
				if (a.uuid.Data4[3] < b.uuid.Data4[3])
					if (a.uuid.Data4[2] < b.uuid.Data4[2])
						if (a.uuid.Data4[1] < b.uuid.Data4[1])
							if (a.uuid.Data4[0] < b.uuid.Data4[0])
								if (a.uuid.Data3 < b.uuid.Data3)
									if (a.uuid.Data2 < b.uuid.Data2)
										if (a.uuid.Data1 < b.uuid.Data1)
											return true;
				return false;
			}
			//Провірка a > b
			__forceinline friend Boolean operator>(const UUID & a, const UUID & b) noexcept {
				if (a.uuid.Data4[3] > b.uuid.Data4[3])
					if (a.uuid.Data4[2] > b.uuid.Data4[2])
						if (a.uuid.Data4[1] > b.uuid.Data4[1])
							if (a.uuid.Data4[0] > b.uuid.Data4[0])
								if (a.uuid.Data3 > b.uuid.Data3)
									if (a.uuid.Data2 > b.uuid.Data2)
										if (a.uuid.Data1 > b.uuid.Data1)
											return true;
				return false;
			}
			//Провірка a>=b
			__forceinline friend Boolean operator>=(const UUID & a, const UUID & b) noexcept { return a == b || a > b; }
			//Провірка a<=b
			__forceinline friend Boolean operator<=(const UUID & a, const UUID & b) noexcept { return a == b || a < b; }
		};
	}
}