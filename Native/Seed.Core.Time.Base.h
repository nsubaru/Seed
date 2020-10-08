#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Time {
			class Date;
			class DateSpan;
			class TimeSpan;
			class DateTime;

			/// <summary>
			/// Кількість інтервалів в секунді
			/// </summary>
			static constexpr UInt64 SecondInNtTime = 1000ULL * 10000ULL;

			/// <summary>
			/// Кількість тіків в мілісекунді
			/// </summary>
			static constexpr UInt64 MiliSecondsTicksCount = SecondInNtTime / 1000ULL;
			/// <summary>
			/// Кількість тіків в секунді
			/// </summary>
			static constexpr UInt64 SecondTicksCount = MiliSecondsTicksCount * 1000ULL;
			/// <summary>
			/// Кількість тіків в хвилині
			/// </summary>
			static constexpr UInt64 MinutesTicksCount = SecondTicksCount * 60ULL;
			/// <summary>
			/// Кількість тіків в годині
			/// </summary>
			static constexpr UInt64 HourTicksCount = MinutesTicksCount * 60ULL;
			/// <summary>
			/// Кількість тіків у дні
			/// </summary>
			static constexpr UInt64 DayTicksCount = HourTicksCount * 24ULL;
			/// <summary>
			/// Кількість тіків в тижні
			/// </summary>
			static constexpr UInt64 WeekTicksCount = DayTicksCount * 7ULL;
			/// <summary>
			/// Кількість тіків у місяці(30 днів)
			/// </summary>
			static constexpr UInt64 MonthTicksCount = DayTicksCount * 30ULL;
			/// <summary>
			/// Кількість тіків у році(366 днів)
			/// </summary>
			static constexpr UInt64 YearTicksCount = DayTicksCount * 366ULL;
			
			/// <summary>
			/// Клас дати
			/// </summary>
			class Date final {
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
				operator DateTime() const;

				~Date() {};
			};

			//Клас відносної дати
			class DateSpan final {
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
				/// Кількість тижнів
				/// </summary>
				UInt16 Weeks;
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
				/// Кількість тіків
				/// </summary>
				UInt16 Ticks;

				/// <summary>
				/// Конструктор з дати і часу
				/// </summary>
				/// <param name="Year">Рік</param>
				/// <param name="Month">Місяць</param>
				/// <param name="Weeks">Кількість тижнів</param>
				/// <param name="Day">День місяця</param>
				/// <param name="Hour">Година</param>
				/// <param name="Minutes">Хвилина</param>
				/// <param name="Seconds">Секунда</param>
				/// <param name="MiliSeconds">Мілісекунда</param>
				/// <param name="Ticks">Кількість тіків</param>
				constexpr DateSpan(
					UInt16 Year = 0,
					UInt16 Month = 0,
					UInt16 Weeks = 0,
					UInt16 Day = 0,
					UInt16 Hour = 0,
					UInt16 Minutes = 0,
					UInt16 Seconds = 0,
					UInt16 MiliSeconds = 0,
					UInt16 Ticks = 0
				) :
					Year(Year),
					Month(Month),
					Weeks(Weeks),
					Day(Day),
					Hour(Hour),
					Minutes(Minutes),
					Second(Seconds),
					MiliSecond(MiliSeconds),
					Ticks(Ticks)
				{};

				/// <summary>
				/// Конструктор з об'єкта класу час
				/// </summary>
				/// <param name="time">Об'єкт класу час</param>
				DateSpan(const DateTime& time);
				/// <summary>
				/// Конвертація в час
				/// </summary>
				/// <returns>Об'єкт часу</returns>
				operator DateTime() const;
			};

			/// <summary>
			/// Клас відрізку часу
			/// </summary>
			class TimeSpan final {
			public:
				UInt64 Ticks;
			public:
				/// <summary>
				/// Конструктор ініціалізує нульльовий відрізок
				/// </summary>
				constexpr TimeSpan() : Ticks(0) {}
				/// <summary>
				/// Конструктор з кількост тіків, створює відрізок з вказаною кількість тіків
				/// </summary>
				/// <param name="ticks">Кількість тіків</param>
				constexpr TimeSpan(UInt64 ticks) : Ticks(ticks) {}
				/// <summary>
				/// Конструктор з відрізку дати, ствоює відрізок з вказаного відрізку дати
				/// </summary>
				/// <param name="dateSpan">Відрізок дати</param>
				constexpr TimeSpan(DateSpan dateSpan) : Ticks(0) {
					Ticks += dateSpan.MiliSecond * MiliSecondsTicksCount;
					Ticks += dateSpan.Second * SecondTicksCount;
					Ticks += dateSpan.Minutes * MinutesTicksCount;
					Ticks += dateSpan.Hour * HourTicksCount;
					Ticks += dateSpan.Day * DayTicksCount;
					Ticks += dateSpan.Weeks * WeekTicksCount;
					Ticks += dateSpan.Month * MonthTicksCount;
					Ticks += dateSpan.Year * YearTicksCount;
				}
			public:
				/// <summary>
				/// Додає тіки(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="ticksCount">Кількість тіків</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddTicks(Int32 ticksCount) {
					if (ticksCount < 0) {
						Ticks -= (-ticksCount);
					}
					else {
						Ticks += ticksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає мілісекунди(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="miliSeconds">Кількість мілісекунд</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddMiliSeconds(Int32 miliSeconds) {
					if (miliSeconds < 0) {
						Ticks -= (-miliSeconds) * MiliSecondsTicksCount;
					}
					else {
						Ticks += miliSeconds * MiliSecondsTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає секунди(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="seconds">Кількість секунд</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddSeconds(Int32 seconds) {
					if (seconds < 0) {
						Ticks -= (-seconds) * SecondTicksCount;
					}
					else {
						Ticks += seconds * SecondTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає хвилини(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="minutes">Кількість хвилин</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddMinutes(Int32 minutes) {
					if (minutes < 0) {
						Ticks -= (-minutes) * MinutesTicksCount;
					}
					else {
						Ticks += minutes * MinutesTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає години(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="hours">Кількість годин</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddHours(Int32 hours) {
					if (hours < 0) {
						Ticks -= (-hours) * HourTicksCount;
					}
					else {
						Ticks += hours * HourTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає дні(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="days">Кількість днів</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddDays(Int32 days) {
					if (days < 0) {
						Ticks -= (-days) * DayTicksCount;
					}
					else {
						Ticks += days * DayTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає тижні(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="weeks">Кількість тижнів</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddWeeks(Int32 weeks) {
					if (weeks < 0) {
						Ticks -= (-weeks) * WeekTicksCount;
					}
					else {
						Ticks += weeks * WeekTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає місяці(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="months">Кількість місяців</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddMonths(Int32 months) {
					if (months < 0) {
						Ticks -= (-months) * MonthTicksCount;
					}
					else {
						Ticks += months * MonthTicksCount;
					}

					return *this;
				}

				/// <summary>
				/// Додає роки(якщо значення від'ємне то віднімає)
				/// </summary>
				/// <param name="years">Кількість років</param>
				/// <returns>Нове значення об'єкта</returns>
				constexpr TimeSpan& AddYears(Int32 years) {
					if (years < 0) {
						Ticks -= (-years) * YearTicksCount;
					}
					else {
						Ticks += years * YearTicksCount;
					}

					return *this;
				}
			public:
				/// <summary>
				/// Вертає кількість мілісекунд
				/// </summary>
				/// <returns>Кількість мілісекунд</returns>
				constexpr Double GetMiliseconds() const {
					return this->Ticks / (MiliSecondsTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість секунд
				/// </summary>
				/// <returns>Кількість секунд</returns>
				constexpr Double GetSeconds() const {
					return this->Ticks / (SecondTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість хвилин
				/// </summary>
				/// <returns>Кількість хвилин</returns>
				constexpr Double GetMinutes() const {
					return this->Ticks / (MinutesTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість годин
				/// </summary>
				/// <returns>Кількість годин</returns>
				constexpr Double GetHours() const {
					return this->Ticks / (HourTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість днів
				/// </summary>
				/// <returns>Кількість днів</returns>
				constexpr Double GetDays() const {
					return this->Ticks / (DayTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість тижнів
				/// </summary>
				/// <returns>Кількість тижнів</returns>
				constexpr Double GetWeeks() const {
					return this->Ticks / (WeekTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість місяців
				/// </summary>
				/// <returns>Кількість місяців</returns>
				constexpr Double GetMonths() const {
					return this->Ticks / (MonthTicksCount * 1.0);
				}

				/// <summary>
				/// Вертає кількість років
				/// </summary>
				/// <returns>Кількість років</returns>
				constexpr Double GetYears() const {
					return this->Ticks / (YearTicksCount * 1.0);
				}
			public:
				/// <summary>
				/// Кількість мілісекунд
				/// </summary>
				Double Property(GetMiliseconds) Miliseconds;

				/// <summary>
				/// Кількість секунд
				/// </summary>
				Double Property(GetSeconds) Seconds;

				/// <summary>
				/// Кількість хвилин
				/// </summary>
				Double Property(GetMinutes) Minutes;

				/// <summary>
				/// Кількість годин
				/// </summary>
				Double Property(GetHours) Hours;

				/// <summary>
				/// Кількість днів
				/// </summary>
				Double Property(GetDays) Days;

				/// <summary>
				/// Кількість тижнів
				/// </summary>
				Double Property(GetWeeks) Weeks;

				/// <summary>
				/// Кількість місяців
				/// </summary>
				Double Property(GetMonths) Months;

				/// <summary>
				/// Кількість років
				/// </summary>
				Double Property(GetYears) Years;

			public:
				/// <summary>
				/// Перетворення в відрізок дат
				/// </summary>
				constexpr operator DateSpan() const {
					UInt64 delta = 0;

					UInt64 yearsCount = Ticks / YearTicksCount;
					delta += yearsCount * YearTicksCount;

					UInt64 monthsCount = (Ticks - delta) / MonthTicksCount;
					delta += monthsCount * MonthTicksCount;

					UInt64 weeksCount = (Ticks - delta) / WeekTicksCount;
					delta += weeksCount * WeekTicksCount;

					UInt64 daysCount = (Ticks - delta) / DayTicksCount;
					delta += daysCount * DayTicksCount;

					UInt64 hoursCount = (Ticks - delta) / HourTicksCount;
					delta += hoursCount * HourTicksCount;

					UInt64 minutesCount = (Ticks - delta) / MinutesTicksCount;
					delta += minutesCount * MinutesTicksCount;

					UInt64 secondsCount = (Ticks - delta) / SecondTicksCount;
					delta += secondsCount * SecondTicksCount;

					UInt64 milisecodsCount = Ticks / MiliSecondsTicksCount;
					UInt64 ticksCount = Ticks % MiliSecondsTicksCount;

					return DateSpan(
						static_cast<UInt16>(yearsCount),
						static_cast<UInt16>(monthsCount),
						static_cast<UInt16>(weeksCount),
						static_cast<UInt16>(daysCount),
						static_cast<UInt16>(hoursCount),
						static_cast<UInt16>(minutesCount),
						static_cast<UInt16>(secondsCount),
						static_cast<UInt16>(milisecodsCount),
						static_cast<UInt16>(ticksCount)
					);
				}
			public:
				/// <summary>
				/// Вертає кількість тіків
				/// </summary>
				/// <returns>Кількість тіків</returns>
				constexpr UInt64 GetTicks() const {
					return this->Ticks;
				}
			};

			/// <summary>
			/// Клас часу оптимізований під швидку арифметику
			/// </summary>
			class DateTime final {
			public:
				UInt64 Ticks = 0;
			public:
				/// <summary>
				/// Пустий конструктор
				/// </summary>
				constexpr DateTime() = default;//-V730
				/// <summary>
				/// Конструктор з задаванням 100 наносекндних інтервалах
				/// </summary>
				constexpr DateTime(UInt64 time) :Ticks(time) {}; //-V730
				/// <summary>
				/// Копіювальний конструктор
				/// </summary>
				constexpr DateTime(const DateTime& orig) :Ticks(orig.Ticks) {}; //-V730
				/// <summary>
				/// Оператор додавання до поточного часу
				/// </summary>
				/// <param name="time">Правий операнд</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				constexpr DateTime& operator+=(DateTime time) { this->Ticks += time.Ticks; return *this; }
				/// <summary>
				/// Оператор віднімання від поточного часу
				/// </summary>
				/// <param name="time">Правий операнд</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				constexpr DateTime& operator-=(DateTime time) { this->Ticks -= time.Ticks; return *this; }
				/// <summary>
				/// Оператор додавання
				/// </summary>
				/// <param name="time1">Лівий операнд</param>
				/// <param name="time2">Правий операнд</param>
				/// <returns>Результат операції</returns>
				constexpr friend DateTime operator+(DateTime time1, DateTime time2) { return time1.Ticks + time2.Ticks; }
				/// <summary>
				/// Оператор віднімання
				/// </summary>
				/// <param name="time1">Лівий операнд</param>
				/// <param name="time2">Правий операнд</param>
				/// <returns>Результат операції</returns>
				constexpr friend DateTime operator-(DateTime time1, DateTime time2) { return time1.Ticks - time2.Ticks; }
				/// <summary>
				/// Вертає кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
				/// </summary>
				/// <returns>Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року</returns>
				constexpr UInt64 GetMsFrom1600() const volatile { return Ticks; }
				/// <summary>
				/// Встановлює час як кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
				/// </summary>
				/// <param name="AbsoluteTime">Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року</param>
				constexpr void SetMsFrom1600(UInt64 AbsoluteTime) volatile { this->Ticks = AbsoluteTime; }
				/// <summary>
				/// Кількість 100 наносекудних інтервалів пройдених з 1 січня 1600 року
				/// </summary>
				UInt64 Property(GetMsFrom1600, SetMsFrom1600) TimeFromStartEpoch;
				/// <summary>
				/// Вертає кількість 100 наносекудних інтервалів у відносному форматі
				/// </summary>
				/// <returns></returns>
				constexpr UInt64 GetAsRelativeTime() const volatile { return -static_cast<Int64>(Ticks); }
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
					this->Ticks = newValue.Ticks; return *this;
				}
				/// <summary>
				/// Оператор присвоєння
				/// </summary>
				/// <param name="newValue">Правий операнд</param>
				/// <returns>Посилання на поточний об'єки</returns>
				volatile DateTime& operator=(const volatile DateTime& newValue) volatile { this->Ticks = newValue.Ticks; return *this; }
			};

			/// <summary>
			/// Діапазон часу
			/// </summary>
			class TimeRange final {
			public:
				/// <summary>
				/// Початок діапазону
				/// </summary>
				DateTime StartTime;
				/// <summary>
				/// Тривалість
				/// </summary>
				TimeSpan Durration;
			public:
				/// <summary>
				/// Пустий конструктор, ініціалізує пустий часовий діапазон
				/// </summary>
				constexpr TimeRange() : StartTime(), Durration() {}

				/// <summary>
				/// Конструктор з початку і тривалості діапазону часу
				/// </summary>
				/// <param name="startTime">Початок діапазону часу</param>
				/// <param name="durration">Тривалість діапазону часу</param>
				constexpr TimeRange(DateTime startTime, TimeSpan durration) : StartTime(startTime), Durration(durration) {}

				/// <summary>
				/// Конструктор з початку і кінця діапазону часу
				/// </summary>
				/// <param name="startTime">Початок діапазону часу</param>
				/// <param name="endTime">Кінець діапазону часу</param>
				constexpr TimeRange(DateTime startTime, DateTime endTime) : StartTime(0ULL), Durration(0ULL) {
					if (startTime.Ticks == endTime.Ticks) {
						this->StartTime = startTime; 
						this->Durration = TimeSpan(0);

					}
					else if (startTime.Ticks < endTime.Ticks) {
						this->StartTime = startTime;
						this->Durration = TimeSpan(endTime.Ticks - startTime.Ticks);
					}
					else {
						this->StartTime = endTime;
						this->Durration = TimeSpan(startTime.Ticks - endTime.Ticks);
					}
				}

				/// <summary>
				/// Конструктор з початку і тривалості діапазону часу
				/// </summary>
				/// <param name="startTime">Початок діапазону часу</param>
				/// <param name="durration">Тривалість діапазону часу</param>
				constexpr TimeRange(DateTime startTime, DateSpan durration) : StartTime(startTime), Durration(durration) {}
			public:
				/// <summary>
				/// Чи діапазон нульової довжини
				/// </summary>
				/// <returns>Логічне значення</returns>
				constexpr Boolean GetIsZeroRange() const { return Durration.Ticks == 0; }

				/// <summary>
				/// Чи діапазон нульової довжини
				/// </summary>
				Boolean Property(GetIsZeroRange) IsZeroRange;
			};
		}

		namespace Internals
		{
			InternalApiCall void ConvertDateTimeToStringImpl(const Seed::Core::Time::DateTime& time, Seed::CharStr buffer);
			InternalApiCall void ConvertDateToStringImpl(Seed::Core::Time::Date date, Seed::Core::CharStr buffer);
		}
	}
}