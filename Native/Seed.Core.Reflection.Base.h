#pragma once
#include "Seed.Core.CTTI.h"
#include "Seed.Core.RTTI.h"
#include "Seed.Core.Serilization.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Reflection
		{
			using NonSerelizableType = Serilization::NonSerelizableType;

			template<typename THeap>
			using BaseTypeMetaDataRecord = Serilization::BaseTypeMetaDataRecord<THeap>;

			using TypeMetaDataRecord = Serilization::TypeMetaDataRecord;
			using SensitiveTypeMetaDataRecord = Serilization::SensitiveTypeMetaDataRecord;

			///<summary>Неможливо викликати динамічну функцію з даним набором аргументів</summary>
			class InvlaidDynamicFunctionArgs : public Exception {
			public:
				InvlaidDynamicFunctionArgs(const InvlaidDynamicFunctionArgs& orig) : Exception(orig) {}
				InvlaidDynamicFunctionArgs(InvlaidDynamicFunctionArgs&& from) : Exception(MoveRValue(from)) {}
				InvlaidDynamicFunctionArgs& operator=(const InvlaidDynamicFunctionArgs& from) { this->~InvlaidDynamicFunctionArgs(); this->InvlaidDynamicFunctionArgs::InvlaidDynamicFunctionArgs(from); return *this; }
				InvlaidDynamicFunctionArgs& operator=(InvlaidDynamicFunctionArgs&& from) { this->~InvlaidDynamicFunctionArgs(); this->InvlaidDynamicFunctionArgs::InvlaidDynamicFunctionArgs(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvlaidDynamicFunctionArgs(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати динамічну функцію з даним набором аргументів") {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(),
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvlaidDynamicFunctionArgs(*this));
				}

				virtual ~InvlaidDynamicFunctionArgs() {}
			};
		}
	}
}