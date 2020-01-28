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
			BaseStringView(const BaseString<Heap>& refStr, SizeT offset = 0, SizeT len = Math::Max<SizeT>()) 
				: refStr(refStr), offset(offset), len(len) {}
			BaseStringView(BaseStringView&) = delete;
			BaseStringView& operator=(BaseStringView&) = delete;
			/// <summary>
			/// Переміщення даних референса в новий об'єкт
			/// </summary>
			/// <param name="from"></param>
			BaseStringView(BaseStringView&& from) : refStr(from.refStr), offset(from.offset), len(from.len) {}
			/// <summary>
			/// Оператор переміщення референса в новий об'єкт
			/// </summary>
			/// <param name="from">Оригинал</param>
			/// <returns>Силку на поточний об'єкт</returns>
			BaseStringView& operator=(BaseStringView&& from) {
				this->~BaseStringView();
				this->BaseStringView::BaseStringView(MoveRValue(from));
			}
		public:
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>true якщо рівні</returns>
			bool operator==(const BaseString<Heap>& other) const {
				if (ActualViewLen != other.Lenght) return false;
				for (SizeT i = 0; i < other.Lenght; i++)
				{
					if (other[i] != refStr[offset + i])
						return false;
				}
				return true;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>false якщо рівні</returns>
			bool operator!=(const BaseString<Heap>& other) const {
				return !(*this == other);
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рівні</returns>
			friend bool operator==(const BaseString<Heap>& first, const BaseStringView& second) {
				return second == first;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second"><Рядок/param>
			/// <returns>true якщо не рівні</returns>
			friend bool operator!=(const BaseString<Heap>& first, const BaseStringView& second) {
				return second != first;
			}
		public:
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>true якщо рядок є більшим</returns>
			bool operator<(const BaseString<Heap>& second) {
				if (ActualViewLen >= second.Lenght) return false;
				for (SizeT i = 0; i < ActualViewLen; i++)
				{
					if (second[i] >= refStr[offset + i])
						return false;
				}
				return true;
			}

			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>true якщо рядок менший</returns>
			bool operator>(const BaseString<Heap>& second) {
				if (ActualViewLen <= second.Lenght) return false;
				for (SizeT i = 0; i < second.Lenght; i++)
				{
					if (second[i] >= refStr[offset + i])
						return false;
				}
				return true;
			}

			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>true якщо рядок більший або рівний</returns>
			bool operator<=(const BaseString<Heap>& second) {
				if (ActualViewLen > second.Lenght) return false;
				for (SizeT i = 0; i < ActualViewLen; i++)
				{
					if (second[i] > refStr[offset + i])
						return false;
				}
				return true;
			}

			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="other">Рядок</param>
			/// <returns>true якщо рядок менший або рівний</returns>
			bool operator>=(const BaseString<Heap>& second) {
				if (ActualViewLen < second.Lenght) return false;
				for (SizeT i = 0; i < second.Lenght; i++)
				{
					if (second[i] < refStr[offset + i])
						return false;
				}
				return true;
			}

			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рядок менший</returns>
			friend bool operator<(const BaseString<Heap>& first, const BaseStringView& second) {
				if (second.ActualViewLen <= first.Lenght) return false;
				for (SizeT i = 0; i < first.Lenght; i++)
				{
					if (first[i] >= second.refStr[second.offset + i])
						return false;
				}
				return true;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рядок є більшим</returns>
			friend bool operator>(const BaseString<Heap>& first, const BaseStringView& second) {
				if (second.ActualViewLen >= first.Lenght) return false;
				for (SizeT i = 0; i < second.ActualViewLen; i++)
				{
					if (first[i] >= second.refStr[second.offset + i])
						return false;
				}
				return true;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рядок менший або рівний</returns>
			friend bool operator<=(const BaseString<Heap>& first, const BaseStringView& second) {
				if (second.ActualViewLen < first.Lenght) return false;
				for (SizeT i = 0; i < first.Lenght; i++)
				{
					if (first.other[i] < second.refStr[second.offset + i])
						return false;
				}
				return true;
			}
			/// <summary>
			/// Порівняння з рядком
			/// </summary>
			/// <param name="first">Відображення рядка</param>
			/// <param name="second">Рядок</param>
			/// <returns>true якщо рядок більший або рівний</returns>
			friend bool operator>=(const BaseString<Heap>& first, const BaseStringView& second) {
				if (second.ActualViewLen > first.Lenght) return false;
				for (SizeT i = 0; i < ActualViewLen; i++)
				{
					if (first[i] > second.refStr[second.offset + i])
						return false;
				}
				return true;
			}
		public:
			/// <summary>
			/// Оператор отримання символу в межах Offset - Offset + ViewLen якщо Індекс рівний Offset + ViewLen то вертаєьться нуль термінатор
			/// </summary>
			/// <param name="Index">Індекс символу</param>
			/// <returns>Літеру яка знаходиться за позицією</returns>
			Char operator[](SizeT Index) const {
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
			const BaseString<Heap>& GetFullText() const {
				return refStr;
			}

			/// <summary>
			/// Вертає оффсет
			/// </summary>
			/// <returns></returns>
			SizeT GetOffset() const {
				return offset;
			}

			/// <summary>
			/// Встановлює новий оффсет
			/// </summary>
			/// <param name="newOffset">Новий оффсет</param>
			void SetOffset(SizeT newOffset) {
				offset = newOffset;
			}

			/// <summary>
			/// Вертає межу референса
			/// </summary>
			/// <returns></returns>
			SizeT GetViewLen() const {
				return len;
			}

			/// <summary>
			/// Встановлює нову межу референса
			/// </summary>
			/// <param name="newViewLen">Нова межа референса</param>
			void SetViewLen(SizeT newViewLen) {
				len = newViewLen;
			}

			/// <summary>
			/// Отримання рядка з відображення
			/// </summary>
			/// <returns></returns>
			BaseString<Heap> ToString() const {
#pragma warning(push)
//Use internal string ctor
#pragma warning(disable: 4996)
				return BaseString<Heap>(((const CharStr)(ConstCharStr) refStr.operator ConstCharStr()) + offset, ActualViewLen);
#pragma warning(pop)
			}

			/// <summary>
			/// Фактичний текст
			/// </summary>
			/// <returns></returns>
			SizeT GetActualViewLen() const {
				return (len == Math::Max<SizeT>() || len + offset > refStr.Lenght) ? refStr.Lenght - offset : len;
			}

			/// <summary>
			/// Фактичний текст
			/// </summary>
			BaseString<Heap> Property(ToString) Text;
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
		};

		///<summary>Простий референс на рядок</summary>
        namespace { using StringView = BaseStringView<Memory::Heap>; }
        ///<summary>Референс на рядок з конфіденційною інформацією</summary>
        namespace { using SensitiveStringView = BaseStringView<Memory::SensitiveDataHeap>; }
	}
}