#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.CTTI.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Serilization
		{
			template<typename TSerelizeable>
			using ISerilizable = CTTI::TypeTraits::ISerilizable<TSerelizeable>;

			///<summary>Не можливо серелізувати або десерелізувати даний тип</summary>
			class NonSerelizableType : public Exception {
			private:
				/// <summary>
				/// Ім'я типу який не знайдено
				/// </summary>
				ExceptionString typeName;
			public:
				NonSerelizableType(const NonSerelizableType& orig) : Exception(orig) {}
				NonSerelizableType(NonSerelizableType&& from) : Exception(MoveRValue(from)) {}
				NonSerelizableType& operator=(const NonSerelizableType& from) { this->~NonSerelizableType(); this->NonSerelizableType::NonSerelizableType(from); return *this; }
				NonSerelizableType& operator=(NonSerelizableType&& from) { this->~NonSerelizableType(); this->NonSerelizableType::NonSerelizableType(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param Name = 'Type'>Тип який DI не зумів знайти</param>
				NonSerelizableType(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ExceptionString&& typeName)
					:Exception(File, FunctName, Line, L"Неможливо серелізувати даний тип"), typeName(MoveRValue(typeName)) {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Ім'я типу: "), typeName, ExceptionString(L". "),
						location.toString(),
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new NonSerelizableType(*this));
				}

				virtual ~NonSerelizableType() {}
			};
		}
	}
}