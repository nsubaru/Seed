#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Math.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Comparing
		{
			/// <summary>
			/// Результат порівняння
			/// </summary>
			namespace
			{
				using ComparingResult = Int8;
			}
			/// <summary>
			/// Нуль тип
			/// </summary>
			namespace
			{
				using LiteralZero = NullptrT;
			}

			/// <summary>
			/// Категорії рівності
			/// </summary>
			enum class ECompareEqual : ComparingResult {
				Equal = 0,
				Equivalent = Equal,
				NonEqual = 1,
				NonEquivalent = NonEqual
			};

			/// <summary>
			/// Категорії порядку
			/// </summary>
			enum class ECompareOrdinal : ComparingResult {
				Less = -1,
				Greater = 1
			};

			/// <summary>
			/// Категорії не впорядкованості
			/// </summary>
			enum class ECompareUnOrderable : ComparingResult {
				UnOrdered = Seed::Math::Min<Seed::Int8>() + 1
			};
		}
	}
}

namespace std
{
	using namespace Seed::Core::Comparing;
	using Seed::Core::Boolean;

	class strong_ordering;
	class partial_ordering;
	class weak_ordering;
	class weak_equality;
	class strong_equality;

#pragma region Weak equality comparing
	/// <summary>
	/// Слабке порівняння
	/// </summary>
	class weak_equality {
		/// <summary>
		/// Значення рівності
		/// </summary>
		ComparingResult value;
	public:
		/// <summary>
		/// Конструктор з категорії рівності
		/// </summary>
		/// <param name="equal">Категорія рівності</param>
		constexpr explicit weak_equality(const ECompareEqual equal) : value(static_cast<ComparingResult>(equal)) {}
	public:
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		friend constexpr Boolean operator==(const weak_equality left, LiteralZero) {
			return left.value == 0;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		friend constexpr Boolean operator!=(const weak_equality left, LiteralZero) {
			return left.value != 0;
		}
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		friend constexpr Boolean operator==(LiteralZero, const weak_equality left) {
			return 0 == left.value;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		friend constexpr Boolean operator!=(LiteralZero, const weak_equality left) {
			return 0 != left.value;
		}
	public:
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		friend constexpr weak_equality operator<=>(const weak_equality left, LiteralZero) {
			return left;
		}
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		friend constexpr weak_equality operator<=>(LiteralZero, const weak_equality left) {
			return left;
		}
	public:
		static const weak_equality equivalent;
		static const weak_equality nonequivalent;
	};

	inline constexpr weak_equality weak_equality::equivalent(ECompareEqual::Equivalent);
	inline constexpr weak_equality weak_equality::nonequivalent(ECompareEqual::NonEquivalent);
#pragma endregion
#pragma region Strong equality comparing
	/// <summary>
	/// Слабке порівняння
	/// </summary>
	class strong_equality {
		/// <summary>
		/// Значення рівності
		/// </summary>
		ComparingResult value;
	public:
		/// <summary>
		/// Конструктор з категорії рівності
		/// </summary>
		/// <param name="equal">Категорія рівності</param>
		constexpr explicit strong_equality(const ECompareEqual equal) : value(static_cast<ComparingResult>(equal)) {}
	public:
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		friend constexpr Boolean operator==(const strong_equality left, LiteralZero) {
			return left.value == 0;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		friend constexpr Boolean operator!=(const strong_equality left, LiteralZero) {
			return left.value != 0;
		}
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		friend constexpr Boolean operator==(LiteralZero, const strong_equality left) {
			return 0 == left.value;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		friend constexpr Boolean operator!=(LiteralZero, const strong_equality left) {
			return 0 != left.value;
		}
	public:
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		friend constexpr strong_equality operator<=>(const strong_equality left, LiteralZero) {
			return left;
		}
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		friend constexpr strong_equality operator<=>(LiteralZero, const strong_equality left) {
			return left;
		}
	public:
		static const strong_equality equal;
		static const strong_equality nonequal;
		static const strong_equality equivalent;
		static const strong_equality nonequivalent;
	};

	inline constexpr strong_equality strong_equality::equal(ECompareEqual::Equal);
	inline constexpr strong_equality strong_equality::nonequal(ECompareEqual::NonEqual);
	inline constexpr strong_equality strong_equality::equivalent(ECompareEqual::Equivalent);
	inline constexpr strong_equality strong_equality::nonequivalent(ECompareEqual::NonEquivalent);

#pragma endregion
#pragma region Patial orderable comparing result
	/// <summary>
	/// Класс результату часткової рівності
	/// </summary>
	class partial_ordering {
	private:
		/// <summary>
		/// Значення рівності
		/// </summary>
		ComparingResult value;
		/// <summary>
		/// Флаг впорядкованості
		/// </summary>
		Boolean isOrdered;
	private:
		/// <summary>
		/// Внутрішній конструктор з значеннь полів
		/// </summary>
		/// <param name="value">Значення рівності</param>
		/// <param name="isOrdered">Флаг впорядкованості</param>
		__forceinline constexpr partial_ordering(ComparingResult value, Boolean isOrdered) : value(value), isOrdered(isOrdered) {}
	public:
		/// <summary>
		/// Конструктор з категорій рівності
		/// </summary>
		/// <param name="equal">Категорії рівності</param>
		__forceinline constexpr partial_ordering(ECompareEqual equal) : value(static_cast<ComparingResult>(equal)), isOrdered(true) {}
		/// <summary>
		/// Конструктор з категорій порядку
		/// </summary>
		/// <param name="equal">Категорії порядку</param>
		__forceinline constexpr partial_ordering(ECompareOrdinal equal) : value(static_cast<ComparingResult>(equal)), isOrdered(true) {}
		/// <summary>
		/// Конструктор з категорій не впорядкованості
		/// </summary>
		/// <param name="equal">Категорії не впорядкованості</param>
		__forceinline constexpr partial_ordering(ECompareUnOrderable equal) : value(static_cast<ComparingResult>(equal)), isOrdered(false) {}
	public:
		/// <summary>
		/// Перетворення в weak_equality
		/// </summary>
		/// <returns>weak_equality результат</returns>
		__forceinline constexpr operator weak_equality() const {
			return weak_equality{ static_cast<ECompareEqual>(value != 0) };
		}
	public:
#pragma region Left side comparing
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator==(const partial_ordering left, LiteralZero) {
			return left.isOrdered && left.value == 0;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator!=(const partial_ordering left, LiteralZero) {
			return !left.isOrdered || left.value != 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator<(const partial_ordering left, LiteralZero) {
			return left.isOrdered && left.value < 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator>(const partial_ordering left, LiteralZero) {
			return left.isOrdered && left.value > 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater або ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator<=(const partial_ordering left, LiteralZero) {
			return left.isOrdered && left.value <= 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less або ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator>=(const partial_ordering left, LiteralZero) {
			return left.isOrdered && left.value >= 0;
		}
#pragma endregion
#pragma region Right side comparing
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator==(LiteralZero, const partial_ordering right) {
			return right.isOrdered && 0 == right.value;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator!=(LiteralZero, const partial_ordering right) {
			return !right.isOrdered || 0 != right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator<(LiteralZero, const partial_ordering right) {
			return right.isOrdered && 0 < right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator>(LiteralZero, const partial_ordering right) {
			return right.isOrdered && 0 > right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater або ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator<=(LiteralZero, const partial_ordering right) {
			return right.isOrdered && 0 <= right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less або ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator>=(LiteralZero, const partial_ordering right) {
			return right.isOrdered && 0 >= right.value;
		}
#pragma endregion
#pragma region Three-way comparing
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		__forceinline constexpr friend partial_ordering operator<=>(const partial_ordering left, LiteralZero) {
			return left;
		}
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		__forceinline constexpr friend partial_ordering operator<=>(LiteralZero, const partial_ordering left) {
			return partial_ordering{ static_cast<ComparingResult>(-left.value),  left.isOrdered };
		}
#pragma endregion
#pragma region Available values
		/// <summary>
		/// Значення при результаті порівняння менше
		/// </summary>
		static const partial_ordering less;
		/// <summary>
		/// Значення при результаті порівняння рівне
		/// </summary>
		static const partial_ordering equivalent;
		/// <summary>
		/// Значення при результаті порівняння більше
		/// </summary>
		static const partial_ordering greater;
		/// <summary>
		/// Значення при результаті порівняння не впордковано
		/// </summary>
		static const partial_ordering unordered;
#pragma endregion
	};

	inline constexpr partial_ordering partial_ordering::less(ECompareOrdinal::Less);
	inline constexpr partial_ordering partial_ordering::equivalent(ECompareEqual::Equivalent);
	inline constexpr partial_ordering partial_ordering::greater(ECompareOrdinal::Greater);
	inline constexpr partial_ordering partial_ordering::unordered(ECompareUnOrderable::UnOrdered);
#pragma endregion
#pragma region Weak orderable comparing result
	/// <summary>
	/// Результат слабого порівняння
	/// </summary>
	class weak_ordering {
	private:
		/// <summary>
		/// Значення результату слабого порівняння
		/// </summary>
		ComparingResult value;
	private:
		/// <summary>
		/// Внутрішній конструктор з значення порівняння
		/// </summary>
		/// <param name="value">Значення порівняння</param>
		__forceinline constexpr weak_ordering(ComparingResult value) : value(value) {}
	public:
		/// <summary>
		/// Конструктор з категорії рівності
		/// </summary>
		/// <param name="equal">Категорія рівності</param>
		__forceinline constexpr weak_ordering(ECompareEqual equal) : value(static_cast<ComparingResult>(equal)) {}
		/// <summary>
		/// Конструктор з категорії порядку
		/// </summary>
		/// <param name="equal">Категорія порядку</param>
		__forceinline constexpr weak_ordering(ECompareOrdinal equal) : value(static_cast<ComparingResult>(equal)) {}
	public:
		/// <summary>
		/// Перетворення до результату часткового порівняння
		/// </summary>
		/// <returns></returns>
		__forceinline constexpr operator partial_ordering() const {
			return partial_ordering{ static_cast<ECompareOrdinal>(this->value) };
		}
	public:
#pragma region Left side comparing
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator==(const weak_ordering left, LiteralZero) {
			return left.value == 0;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator!=(const weak_ordering left, LiteralZero) {
			return left.value != 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator<(const weak_ordering left, LiteralZero) {
			return left.value < 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator>(const weak_ordering left, LiteralZero) {
			return left.value > 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater або ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator<=(const weak_ordering left, LiteralZero) {
			return left.value <= 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less або ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator>=(const weak_ordering left, LiteralZero) {
			return left.value >= 0;
		}
#pragma endregion
#pragma region Right side comparing
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator==(LiteralZero, const weak_ordering right) {
			return 0 == right.value;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator!=(LiteralZero, const weak_ordering right) {
			return 0 != right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator<(LiteralZero, const weak_ordering right) {
			return 0 < right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator>(LiteralZero, const weak_ordering right) {
			return 0 > right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater або ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator<=(LiteralZero, const weak_ordering right) {
			return 0 <= right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less або ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator>=(LiteralZero, const weak_ordering right) {
			return 0 >= right.value;
		}
#pragma endregion
#pragma region Three-way comparing
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		__forceinline constexpr friend weak_ordering operator<=>(const weak_ordering left, LiteralZero) {
			return left;
		}
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		__forceinline constexpr friend weak_ordering operator<=>(LiteralZero, const weak_ordering left) {
			return weak_ordering{ static_cast<ComparingResult>(-left.value) };
		}
#pragma endregion
#pragma region Available values
		/// <summary>
		/// Значення при результаті порівняння менше
		/// </summary>
		static const weak_ordering less;
		/// <summary>
		/// Значення при результаті порівняння еквівалентний
		/// </summary>
		static const weak_ordering equivalent;
		/// <summary>
		/// Значення при результаті порівняння більше
		/// </summary>
		static const weak_ordering greater;
#pragma	endregion
	};

	inline constexpr weak_ordering weak_ordering::less(ECompareOrdinal::Less);
	inline constexpr weak_ordering weak_ordering::equivalent(ECompareEqual::Equivalent);
	inline constexpr weak_ordering weak_ordering::greater(ECompareOrdinal::Greater);
#pragma endregion
#pragma region Strong orderable comparing
	/// <summary>
	/// Результат слабого порівняння
	/// </summary>
	class strong_ordering {
	private:
		/// <summary>
		/// Значення результату слабого порівняння
		/// </summary>
		ComparingResult value;
	private:
		/// <summary>
		/// Внутрішній конструктор з значення порівняння
		/// </summary>
		/// <param name="value">Значення порівняння</param>
		__forceinline constexpr strong_ordering(ComparingResult value) : value(value) {}
	public:
		/// <summary>
		/// Конструктор з категорії рівності
		/// </summary>
		/// <param name="equal">Категорія рівності</param>
		__forceinline constexpr strong_ordering(ECompareEqual equal) : value(static_cast<ComparingResult>(equal)) {}
		/// <summary>
		/// Конструктор з категорії порядку
		/// </summary>
		/// <param name="equal">Категорія порядку</param>
		__forceinline constexpr strong_ordering(ECompareOrdinal equal) : value(static_cast<ComparingResult>(equal)) {}
	public:
		/// <summary>
		/// Перетворення до результату часткового порівняння
		/// </summary>
		/// <returns>PartialOrdering</returns>
		__forceinline constexpr operator partial_ordering() const {
			return partial_ordering{ static_cast<ECompareOrdinal>(this->value) };
		}
		/// <summary>
		/// Перетворення до результату слабого порівняння
		/// </summary>
		/// <returns>WeakOrdering</returns>
		__forceinline constexpr operator weak_ordering() const {
			return weak_ordering{ static_cast<ECompareOrdinal>(this->value) };
		}
		/// <summary>
		/// Перетворення до результату слабкої рівності
		/// </summary>
		/// <returns>weak_equality</returns>
		__forceinline constexpr operator weak_equality() const {
			return weak_equality{ static_cast<ECompareEqual>(this->value) };
		}
		/// <summary>
		/// Перетворення до результату строго рівності
		/// </summary>
		/// <returns>StrongEquality</returns>
		__forceinline constexpr operator strong_equality() const {
			return strong_equality{ static_cast<ECompareEqual>(this->value) };
		}
	public:
#pragma region Left side comparing
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator==(const strong_ordering left, LiteralZero) {
			return left.value == 0;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator!=(const strong_ordering left, LiteralZero) {
			return left.value != 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator<(const strong_ordering left, LiteralZero) {
			return left.value < 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator>(const strong_ordering left, LiteralZero) {
			return left.value > 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater або ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator<=(const strong_ordering left, LiteralZero) {
			return left.value <= 0;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less або ECompareEqual::Equal 
		/// </summary>
		/// <param name="left">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator>=(const strong_ordering left, LiteralZero) {
			return left.value >= 0;
		}
#pragma endregion
#pragma region Right side comparing
		/// <summary>
		/// Рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator==(LiteralZero, const strong_ordering right) {
			return 0 == right.value;
		}
		/// <summary>
		/// Не рівність з ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння не Equal або Equality</returns>
		__forceinline constexpr friend Boolean operator!=(LiteralZero, const strong_ordering right) {
			return 0 != right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator<(LiteralZero, const strong_ordering right) {
			return 0 < right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater</returns>
		__forceinline constexpr friend Boolean operator>(LiteralZero, const strong_ordering right) {
			return 0 > right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Greater або ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator<=(LiteralZero, const strong_ordering right) {
			return 0 <= right.value;
		}
		/// <summary>
		/// Порівняння результату з ECompareOrdinal::Less або ECompareEqual::Equal 
		/// </summary>
		/// <param name="right">Значення впорядкованості</param>
		/// <returns>true якщо результат порівняння Greater або Equal</returns>
		__forceinline constexpr friend Boolean operator>=(LiteralZero, const strong_ordering right) {
			return 0 >= right.value;
		}
#pragma endregion
#pragma region Three-way comparing
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		__forceinline constexpr friend strong_ordering operator<=>(const strong_ordering left, LiteralZero) {
			return left;
		}
		/// <summary>
		/// Універсальне порівнянння результату з нулем
		/// </summary>
		/// <param name="left">Результат порівняння</param>
		/// <returns>Результат порівняння</returns>
		__forceinline constexpr friend strong_ordering operator<=>(LiteralZero, const strong_ordering left) {
			return strong_ordering{ static_cast<ComparingResult>(-left.value) };
		}
#pragma endregion
#pragma region Available values
		/// <summary>
		/// Значення при результаті порівняння менше
		/// </summary>
		static const strong_ordering less;
		/// <summary>
		/// Значення при результаті порівняння еквівалентний
		/// </summary>
		static const strong_ordering equivalent;
		/// <summary>
		/// Значення при результаті порівняння рівне
		/// </summary>
		static const strong_ordering equal;
		/// <summary>
		/// Значення при результаті порівняння більше
		/// </summary>
		static const strong_ordering greater;
#pragma	endregion
	};

	inline constexpr strong_ordering strong_ordering::less(ECompareOrdinal::Less);
	inline constexpr strong_ordering strong_ordering::equivalent(ECompareEqual::Equivalent);
	inline constexpr strong_ordering strong_ordering::equal(ECompareEqual::Equal);
	inline constexpr strong_ordering strong_ordering::greater(ECompareOrdinal::Greater);
#pragma endregion
}

namespace Seed
{
	inline namespace Core
	{
		namespace Comparing
		{
			using WeakEquality = std::weak_equality;
			using StrongOrdering = std::strong_ordering;
			using WeakOrdering = std::weak_ordering;
			using PartialOrdering = std::partial_ordering;
			using StrongEquality = std::strong_equality;
		}

		using WeakEquality = std::weak_equality;
		using StrongOrdering = std::strong_ordering;
		using WeakOrdering = std::weak_ordering;
		using PartialOrdering = std::partial_ordering;
		using StrongEquality = std::strong_equality;
	}
}
