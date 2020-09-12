#pragma once
#include "Seed.Core.String.h"

namespace Seed
{
	inline namespace Core
	{
		/// <summary>
		/// Референс на константний рядок, дозволяє абстрактно працювти з частиною рядка 
		/// </summary>
		/// <typeparam name="Heap">Тип кучі пам'яті</typeparam>
		template<typename Heap>
		class BaseStringView : Memory::Allocator<Heap> {
		private:
			/// <summary>
			/// Референс на рядок
			/// </summary>
			const BaseString<Heap>& refStr;
			/// <summary>
			/// Відступ від фактично початку тексту в кількості символів
			/// </summary>
			SizeT offset;
			/// <summary>
			/// Довжина частини рядку яку виднов даному об'єкті
			/// </summary>
			SizeT len;
		public:
			/// <summary>
			/// Конструктор з референса рядка і можливо початку і кінця даної частини
			/// </summary>
			/// <param name="refStr">Референс рядка</param>
			/// <param name="offset">Відступ</param>
			/// <param name="len">Довжина частини</param>
			ForceInline BaseStringView(const BaseString<Heap>& refStr, SizeT offset = 0, SizeT len = Math::Max<SizeT>()) 
				: refStr(refStr), offset(offset), len(len) {}
			ForceInline BaseStringView(BaseStringView&) = delete;
			ForceInline BaseStringView& operator=(BaseStringView&) = delete;
			/// <summary>
			/// Переміщення даних референса в новий об'єкт
			/// </summary>
			/// <param name="from"></param>
			ForceInline BaseStringView(BaseStringView&& from) : refStr(from.refStr), offset(from.offset), len(from.len) {}
			/// <summary>
			/// Оператор переміщення референса в новий об'єкт
			/// </summary>
			/// <param name="from">Оригинал</param>
			/// <returns>Силку на поточний об'єкт</returns>
			ForceInline BaseStringView& operator=(BaseStringView&& from) {
				this->~BaseStringView();
				this->BaseStringView::BaseStringView(MoveRValue(from));
			}
		public:
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>true якщо рівні</returns>
			ForceInline bool operator==(const BaseString<Heap>& other) const {
				return (*this <=> other) == 0;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>false якщо рівні</returns>
			ForceInline bool operator!=(const BaseString<Heap>& other) const {
				return !(*this == other);
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рівні</returns>
			ForceInline friend bool operator==(const BaseString<Heap>& first, const BaseStringView& second) {
				return second == first;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second"><Рядок/param>
			/// <returns>true якщо не рівні</returns>
			ForceInline friend bool operator!=(const BaseString<Heap>& first, const BaseStringView& second) {
				return second != first;
			}
		public:
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рівні</returns>
			template<typename TOtherHeap>
			ForceInline friend bool operator==(const BaseString<Heap>& first, const BaseStringView<TOtherHeap>& second) {
				return (second <=> first) == 0;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second"><Рядок/param>
			/// <returns>true якщо не рівні</returns>
			template<typename TOtherHeap>
			ForceInline friend bool operator!=(const BaseString<Heap>& first, const BaseStringView<TOtherHeap>& second) {
				return (second != first) == 0;
			}

			/// <summary>
			/// Універсальне порівняння
			/// </summary>
			/// <param name="first">Перший елемент</param>
			/// <param name="second">Другий елемент</param>
			/// <returns>Результат порівняння</returns>
			ForceInline friend StrongOrdering operator<=>(const BaseString<Heap>& first, const BaseStringView& second) {
				auto comparableLen = Min(first.Lenght, second.ActualViewLen);
				for (SizeT i = 0; i < comparableLen; i++)
				{
					if (first[i] != second.refStr[second.offset + i])
						return first[i] <=> second.refStr[second.offset + i];
				}
				return first.Lenght <=> second.ActualViewLen;
			}

			/// <summary>
			/// Універсальне порівняння
			/// </summary>
			/// <param name="first">Перший елемент</param>
			/// <param name="second">Другий елемент</param>
			/// <returns>Результат порівняння</returns>
			ForceInline friend StrongOrdering operator<=>(const BaseStringView& first, const BaseString<Heap>& second) {
				auto comparableLen = Min(first.ActualViewLen, second.Lenght);
				for (SizeT i = 0; i < comparableLen; i++)
				{
					if (first.refStr[first.offset + i] != second[i])
						return first.refStr[first.offset + i] <=> second[i];
				}
				return first.ActualViewLen <=> second.Lenght;
			}

			/// <summary>
			/// Універсальне порівняння
			/// </summary>
			/// <param name="first">Перший елемент</param>
			/// <param name="second">Другий елемент</param>
			/// <returns>Результат порівняння</returns>
			template<typename TOtherHeap>
			ForceInline friend StrongOrdering operator<=>(const BaseStringView& first, const BaseStringView<TOtherHeap>& second) {
				auto comparableLen = Min(first.ActualViewLen, second.ActualViewLen);
				for (SizeT i = 0; i < comparableLen; i++)
				{
					if (first.refStr[first.offset + i] != second.refStr[second.offset + i])
						return first.refStr[first.offset + i] <=> second.refStr[second.offset + i];
				}
				return first.ActualViewLen <=> second.ActualViewLen;
			}
		public:
			/// <summary>
			/// Оператор отримання символу в межах Offset - Offset + ViewLen якщо Індекс рівний Offset + ViewLen то вертаєьться нуль термінатор
			/// </summary>
			/// <param name="Index">Індекс символу</param>
			/// <returns>Літеру яка знаходиться за позицією</returns>
			ForceInline Char operator[](SizeT Index) const {
				if (offset + Index > offset + len && offset + Index > refStr.Lenght)
				{
					Seed::Core::Internals::Exceptions::ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, offset + Index, refStr.Lenght - len);
				}
				else if(offset + Index == refStr.Lenght + offset)
				{
					return '\0';
				}

				return refStr[offset + Index];
			}

			/// <summary>
			/// Вертає силку на рядок на якому базуєтсья референс
			/// </summary>
			/// <returns></returns>
			ForceInline const BaseString<Heap>& GetFullText() const {
				return refStr;
			}

			/// <summary>
			/// Вертає оффсет
			/// </summary>
			/// <returns></returns>
			ForceInline SizeT GetOffset() const {
				return offset;
			}

			/// <summary>
			/// Встановлює новий оффсет
			/// </summary>
			/// <param name="newOffset">Новий оффсет</param>
			ForceInline void SetOffset(SizeT newOffset) {
				offset = newOffset;
			}

			/// <summary>
			/// Вертає межу референса
			/// </summary>
			/// <returns></returns>
			ForceInline SizeT GetViewLen() const {
				return len;
			}

			/// <summary>
			/// Встановлює нову межу референса
			/// </summary>
			/// <param name="newViewLen">Нова межа референса</param>
			ForceInline void SetViewLen(SizeT newViewLen) {
				len = newViewLen;
			}

			/// <summary>
			/// Отримання рядка з відображення
			/// </summary>
			/// <returns></returns>
			template<typename UHeap = Heap> 
			ForceInline BaseString<UHeap> ToString() const {
#pragma warning(push)
//Use internal string ctor
#pragma warning(disable: 4996)
				return BaseString<UHeap>(((const CharStr) refStr.operator ConstCharStr()) + offset, ActualViewLen);
#pragma warning(pop)
			}

			/// <summary>
			/// Отримання рядка з відображення
			/// </summary>
			/// <returns></returns>
			ForceInline BaseString<Heap> GetText() const {
				return this->ToString();
			}

			/// <summary>
			/// Фактичний текст
			/// </summary>
			/// <returns></returns>
			ForceInline SizeT GetActualViewLen() const {
				return (len == Math::Max<SizeT>() || len + offset > refStr.Lenght) ? refStr.Lenght - offset : len;
			}

			/// <summary>
			/// Рядок з відображення
			/// </summary>
			BaseString<Heap> Property(GetText) Text;
			/// <summary>
			/// Референсний рядок
			/// </summary>
			BaseString<Heap> Property(GetFullText) FullText;
			/// <summary>
			/// Оффсет
			/// </summary>
			SizeT Property(GetOffset, SetOffset) Offset;
			/// <summary>
			/// Межа референса
			/// </summary>
			SizeT Property(GetViewLen, SetViewLen) ViewLen;
			/// <summary>
			/// Фактичний розмір відображення
			/// </summary>
			SizeT Property(GetActualViewLen) ActualViewLen;
		public:
			template<typename TOtherHeap>
			friend class BaseStringView;
		};

		///<summary>Простий референс на рядок</summary>
        namespace { using StringView = BaseStringView<Memory::Heap>; }
        ///<summary>Референс на рядок з конфіденційною інформацією</summary>
        namespace { using SensitiveStringView = BaseStringView<Memory::SensitiveDataHeap>; }
	}
}