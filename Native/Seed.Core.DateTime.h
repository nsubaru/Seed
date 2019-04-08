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
			Date() noexcept :Year(1600), Month(0), Week(0), Day(0), Hour(0), Minutes(0), Second(0), MiliSecond(0) {};
			//Конструктор з дати і часу
			Date(UInt16 Year, UInt16 Month, UInt16 Week, UInt16 Day,
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
			UInt64 time_ms;
		public:
			//Пустий конструктор
			DateTime() noexcept :time_ms(0) {};
			//Конструктор з задаванням 100 наносекндних інтервалах
			DateTime(UInt64 time) :time_ms(time) {};
			//Копіювальний конструктор
			DateTime(const DateTime& orig) :time_ms(orig.time_ms) {};
			//Оператор присвоєння
			DateTime& operator=(const DateTime& from) { this->time_ms = from.time_ms; return *this; }
			//Оператор додавання до
			DateTime& operator+=(DateTime time) { this->time_ms += time.time_ms; return *this; }
			//Оператор віднімання від
			DateTime& operator-=(DateTime time) { this->time_ms -= time.time_ms; return *this; }
			//Оператор додавання
			friend DateTime operator+(DateTime time1, DateTime time2) { return time1.time_ms + time2.time_ms; }
			//Оператор віднімання
			friend DateTime operator-(DateTime time1, DateTime time2) { return time1.time_ms - time2.time_ms; }
			//Вертає кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			UInt64 GetMsFrom1600() { return time_ms; }
			//Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			UInt64 Property(GetMsFrom1600) TimeFromStartEpoch;
			//Вертає кількість 100 наносекудних інтервалів
			UInt64 GetAsRelativeTime() { return -(Int64)time_ms; }
			//Кількість 100 наносекудних інтервалів
			UInt64 Property(GetAsRelativeTime) GetRelatimeTime;
			//Вертає поточний системний час
			//по грінвінському часовому поясі
			static DateTime GetCurrentSystemTimeData();
			//Вертає поточний локальний час
			//в часовому поясі користувача чи системи
			static DateTime GetCurrentLocalTimeData();
			//Деструктор
			~DateTime() {};
			//Конвертуючий конструктор
			friend Date::Date(const DateTime& time) noexcept;

			static constexpr UInt64 SecondInNtTime = 1000ULL * 10000ULL;
		};
	}
}