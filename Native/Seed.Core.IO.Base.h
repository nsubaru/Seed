#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Containers.h"
#include "Seed.Core.String.h"
#include "Seed.Core.CTTI.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			/// <summary>
			/// Загальна I/O помилка
			/// </summary>
			class IoException : public Exception {
			public:
				IoException(const IoException& orig) : Exception(orig) {}
				IoException(IoException&& from) : Exception(MoveRValue(from)) {}
				IoException& operator=(const IoException& from) { this->~IoException(); this->IoException::IoException(from); return *this; }
				IoException& operator=(IoException&& from) { this->~IoException(); this->IoException::IoException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				ForceInline IoException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
					: Exception(File, FuncName, Line, L"General I/O exception") {}

				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Msg'>Текст помилки</param>
				ForceInline IoException(const ConstAString& File, const ConstAString& FuncName, SizeT Line, const ConstString& Msg)
					: Exception(File, FuncName, Line, Msg) {}
			public:
				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						location.toString(),
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new IoException(*this));
				}
			public:
				virtual ~IoException() {}
			};
		}
	}
}