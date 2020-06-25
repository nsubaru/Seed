#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		//Клас часу
		class SeedKernelCall DateTime;

		/// <summary>
		/// Клас дати
		/// </summary>
		class SeedKernelCall Date {
		public:
			/// <summary>
			/// Рік
			/// </summary>
			UInt16 Year;
			/// <summary>
			/// Місяць
			/// </summary>
			UInt16 Month;
			/// <summary>
			/// День тижня
			/// </summary>
			UInt16 WeekDay;
			/// <summary>
			/// День місяця
			/// </summary>
			UInt16 Day;
			/// <summary>
			/// Година
			/// </summary>
			UInt16 Hour;
			/// <summary>
			/// Хвилина
			/// </summary>
			UInt16 Minutes;
			/// <summary>
			/// Секунда
			/// </summary>
			UInt16 Second;
			/// <summary>
			/// Мілісекунда
			/// </summary>
			UInt16 MiliSecond;

			/// <summary>
			/// Пустий конструктор, ініціалізує даьу 1600.0.0 0:0:0.0
			/// </summary>
			constexpr Date() noexcept :Year(1600), Month(0), WeekDay(0), Day(0), Hour(0), Minutes(0), Second(0), MiliSecond(0) {};
			/// <summary>
			/// Конструктор з дати і часу
			/// </summary>
			/// <param name="Year">Рік</param>
			/// <param name="Month">Місяць</param>
			/// <param name="WeekDay">День тижня</param>
			/// <param name="Day">День місяця</param>
			/// <param name="Hour">Година</param>
			/// <param name="Minutes">Хвилина</param>
			/// <param name="Seconds">Секунда</param>
			/// <param name="MiliSeconds">Мілісекунда</param>
			constexpr Date(UInt16 Year, UInt16 Month, UInt16 WeekDay, UInt16 Day,
				UInt16 Hour, UInt16 Minutes, UInt16 Seconds, UInt16 MiliSeconds)
				noexcept :Year(Year), Month(Month), WeekDay(WeekDay), Day(Day),
				Hour(Hour), Minutes(Minutes), Second(Seconds), MiliSecond(MiliSeconds) {};

			/// <summary>
			/// Конструктор з об'єкта класу час
			/// </summary>
			/// <param name="time">Об'єкт класу час</param>
			Date(const DateTime& time);
			/// <summary>
			/// Конвертація в час
			/// </summary>
			/// <returns>Об'єкт часу</returns>
			operator Seed::Core::DateTime() const;

			~Date() {};
		};

		//Клас відносної дати
		class SeedKernelCall DateSpan {
		public:
			/// <summary>
			/// Рік
			/// </summary>
			UInt16 Year;
			/// <summary>
			/// Місяць
			/// </summary>
			UInt16 Month;
			/// <summary>
			/// День тижня
			/// </summary>
			UInt16 WeekDay;
			/// <summary>
			/// День місяця
			/// </summary>
			UInt16 Day;
			/// <summary>
			/// Година
			/// </summary>
			UInt16 Hour;
			/// <summary>
			/// Хвилина
			/// </summary>
			UInt16 Minutes;
			/// <summary>
			/// Секунда
			/// </summary>
			UInt16 Second;
			/// <summary>
			/// Мілісекунда
			/// </summary>
			UInt16 MiliSecond;

			/// <summary>
			/// Пустий конструктор, ініціалізує даьу 1600.0.0 0:0:0.0
			/// </summary>
			constexpr DateSpan() noexcept :Year(1600), Month(0), WeekDay(0), Day(0), Hour(0), Minutes(0), Second(0), MiliSecond(0) {};
			
			/// <summary>
			/// Конструктор з дати і часу
			/// </summary>
			/// <param name="Year">Рік</param>
			/// <param name="Month">Місяць</param>
			/// <param name="WeekDay">День тижня</param>
			/// <param name="Day">День місяця</param>
			/// <param name="Hour">Година</param>
			/// <param name="Minutes">Хвилина</param>
			/// <param name="Seconds">Секунда</param>
			/// <param name="MiliSeconds">Мілісекунда</param>
			constexpr DateSpan(UInt16 Year, UInt16 Month, UInt16 WeekDay, UInt16 Day,
				UInt16 Hour, UInt16 Minutes, UInt16 Seconds, UInt16 MiliSeconds)
				noexcept :Year(Year), Month(Month), WeekDay(WeekDay), Day(Day),
				Hour(Hour), Minutes(Minutes), Second(Seconds), MiliSecond(MiliSeconds) {};

			/// <summary>
			/// Конструктор з об'єкта класу час
			/// </summary>
			/// <param name="time">Об'єкт класу час</param>
			DateSpan(const DateTime& time);
			/// <summary>
			/// Конвертація в час
			/// </summary>
			/// <returns>Об'єкт часу</returns>
			operator Seed::Core::DateTime() const;
			~DateSpan() {};
		};

		/// <summary>
		/// Клас часу оптимізований під швидку арифметику
		/// </summary>
		class SeedKernelCall DateTime {
		private:
			UInt64 time_ms = 0;
		public:
			/// <summary>
			/// Пустий конструктор
			/// </summary>
			constexpr DateTime() = default;//-V730
			/// <summary>
			/// Конструктор з задаванням 100 наносекндних інтервалах
			/// </summary>
			constexpr DateTime(UInt64 time) :time_ms(time) {}; //-V730
			/// <summary>
			/// Копіювальний конструктор
			/// </summary>
			constexpr DateTime(const DateTime& orig) :time_ms(orig.time_ms) {}; //-V730
			/// <summary>
			/// Оператор додавання до поточного часу
			/// </summary>
			/// <param name="time">Правий операнд</param>
			/// <returns>Посилання на поточний об'єкт</returns>
			constexpr DateTime& operator+=(DateTime time) { this->time_ms += time.time_ms; return *this; }
			/// <summary>
			/// Оператор віднімання від поточного часу
			/// </summary>
			/// <param name="time">Правий операнд</param>
			/// <returns>Посилання на поточний об'єкт</returns>
			constexpr DateTime& operator-=(DateTime time) { this->time_ms -= time.time_ms; return *this; }
			/// <summary>
			/// Оператор додавання
			/// </summary>
			/// <param name="time1">Лівий операнд</param>
			/// <param name="time2">Правий операнд</param>
			/// <returns>Результат операції</returns>
			constexpr friend DateTime operator+(DateTime time1, DateTime time2) { return time1.time_ms + time2.time_ms; }
			/// <summary>
			/// Оператор віднімання
			/// </summary>
			/// <param name="time1">Лівий операнд</param>
			/// <param name="time2">Правий операнд</param>
			/// <returns>Результат операції</returns>
			constexpr friend DateTime operator-(DateTime time1, DateTime time2) { return time1.time_ms - time2.time_ms; }
			/// <summary>
			/// Вертає кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			/// </summary>
			/// <returns>Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року</returns>
			constexpr UInt64 GetMsFrom1600() const volatile { return time_ms; }
			/// <summary>
			/// Встановлює час як кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			/// </summary>
			/// <param name="AbsoluteTime">Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року</param>
			constexpr void SetMsFrom1600(UInt64 AbsoluteTime) volatile { this->time_ms = AbsoluteTime; }
			/// <summary>
			/// Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
			/// </summary>
			UInt64 Property(GetMsFrom1600, SetMsFrom1600) TimeFromStartEpoch;
			/// <summary>
			/// Вертає кількість 100 наносекудних інтервалів у відносному форматі
			/// </summary>
			/// <returns></returns>
			constexpr UInt64 GetAsRelativeTime() const volatile { return -static_cast<Int64>(time_ms); }
			/// <summary>
			/// Кількість 100 наносекудних інтервалів у відносному форматі
			/// </summary>
			UInt64 Property(GetAsRelativeTime) RelativeTime;
			/// <summary>
			/// Вертає поточний системний час по грінвінському часовому поясі
			/// </summary>
			/// <returns>Поточний системний час по грінвінському часовому поясі</returns>
			static DateTime GetCurrentSystemTimeData();
			/// <summary>
			/// Вертає поточний локальний час в часовому поясі користувача чи системи
			/// </summary>
			/// <returns>Поточний локальний час в часовому поясі користувача чи системи</returns>
			static DateTime GetCurrentLocalTimeData();
			/// <summary>
			/// Конвертуючий конструктор
			/// </summary>
			friend Date::Date(const DateTime& time);
			/// <summary>
			/// Конвертуючий конструктор
			/// </summary>
			friend DateSpan::DateSpan(const DateTime& time);
			/// <summary>
			/// Оператор присвоєння
			/// </summary>
			/// <param name="newValue">Правий операнд</param>
			/// <returns>Посилання на поточний об'єки</returns>
			DateTime& operator=(DateTime newValue) {
				 this->time_ms = newValue.time_ms; return *this;
			}
			/// <summary>
			/// Оператор присвоєння
			/// </summary>
			/// <param name="newValue">Правий операнд</param>
			/// <returns>Посилання на поточний об'єки</returns>
			volatile DateTime& operator=(const volatile DateTime& newValue) volatile { this->time_ms = newValue.time_ms; return *this; }

			/// <summary>
			/// Кількість інтервалів в секунді
			/// </summary>
			static constexpr UInt64 SecondInNtTime = 1000ULL * 10000ULL;
		};

		namespace Internals
		{
			InternalApiCall SeedKernelCall void ConvertDateTimeToStringImpl(const Seed::Core::DateTime& time, Seed::CharStr buffer);
			InternalApiCall SeedKernelCall void ConvertDateToStringImpl(Seed::Core::Date date, Seed::Core::CharStr buffer);
		}
	}
}