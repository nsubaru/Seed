#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		//Клас часу
		class SeedKernelCall DateTime;

		//Клас дати
		class SeedKernelCall Date {
		public:
			//Окремі поля
			UInt16 Year, Month, Week, Day, Hour, Minutes, Second, MiliSecond;
			//Конструктор
			constexpr Date() noexcept :Year(1600), Month(0), Week(0), Day(0), Hour(0), Minutes(0), Second(0), MiliSecond(0) {};
			//Конструктор з дати і часу
			constexpr Date(UInt16 Year, UInt16 Month, UInt16 Week, UInt16 Day,
				UInt16 Hour, UInt16 Minutes, UInt16 Seconds, UInt16 MiliSeconds)
				noexcept :Year(Year), Month(Month), Week(Week), Day(Day),
				Hour(Hour), Minutes(Minutes), Second(Seconds), MiliSecond(MiliSeconds) {};

			//Конструктор з об'єкта клас час
			Date(const DateTime& time) noexcept;
			//Конвертація в час
			operator DateTime() const noexcept;
			~Date() {};
		};

		//Клас часу оптимізований під швидку арифметику
		class SeedKernelCall DateTime {
		private:
			UInt64 time_ms = 0;
		public:
			//Пустий конструктор
			constexpr DateTime() = default;//-V730
			//Конструктор з задаванням 100 наносекндних інтервалах
			constexpr DateTime(UInt64 time) :time_ms(time) {}; //-V730
			//Копіювальний конструктор
			constexpr DateTime(const DateTime& orig) :time_ms(orig.time_ms) {}; //-V730
			//Оператор присвоєння
			//constexpr DateTime& operator=(const DateTime& from) { this->time_ms = from.time_ms; return *this; }
			//Оператор додавання до
			constexpr DateTime& operator+=(const DateTime& time) { this->time_ms += time.time_ms; return *this; }
			//Оператор віднімання від
			constexpr DateTime& operator-=(const DateTime& time) { this->time_ms -= time.time_ms; return *this; }
			//Оператор додавання
			constexpr friend DateTime operator+(const DateTime & time1, const DateTime & time2) { return time1.time_ms + time2.time_ms; }
			//Оператор віднімання
			constexpr friend DateTime operator-(const DateTime & time1, const DateTime & time2) { return time1.time_ms - time2.time_ms; }
			//Вертає кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			constexpr UInt64 GetMsFrom1600() const volatile { return time_ms; }
			constexpr void SetMsFrom1600(UInt64 AbsoluteTime) volatile { this->time_ms = AbsoluteTime; }
			//Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			UInt64 Property(GetMsFrom1600, SetMsFrom1600) TimeFromStartEpoch;
			//Вертає кількість 100 наносекудних інтервалів
			constexpr UInt64 GetAsRelativeTime() const { return -static_cast<Int64>(time_ms); }
			//Кількість 100 наносекудних інтервалів
			UInt64 Property(GetAsRelativeTime) RelativeTime;
			//Вертає поточний системний час
			//по грінвінському часовому поясі
			static DateTime GetCurrentSystemTimeData();
			//Вертає поточний локальний час
			//в часовому поясі користувача чи системи
			static DateTime GetCurrentLocalTimeData();
			//Конвертуючий конструктор
			friend Date::Date(const DateTime& time) noexcept;

			volatile DateTime& operator=(const volatile DateTime& NewValue) volatile { this->time_ms = NewValue.time_ms; return *this; }

			static constexpr UInt64 SecondInNtTime = 1000ULL * 10000ULL;
		};
	}
}