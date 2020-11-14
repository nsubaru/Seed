#pragma once
#include "Seed.Core.IO.Base.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace IPC {
				/// <summary>
				/// Загальна помилка IPC
				/// </summary>
				class IpcException : public IoException {
				public:
					IpcException(const IpcException& orig) : IoException(orig) {}
					IpcException(IpcException&& from) : IoException(MoveRValue(from)) {}
					IpcException& operator=(const IpcException& from) { this->~IpcException(); this->IpcException::IpcException(from); return *this; }
					IpcException& operator=(IpcException&& from) { this->~IpcException(); this->IpcException::IpcException(MoveRValue(from)); return *this; }
				public:
					///<summary>Конструктор помилки</summary>
					///<param name = 'File'>Файл в якому згенерована помилка</param>
					///<param name = 'FunctName'>Ім'я функції</param>
					///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
					ForceInline IpcException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
						: IoException(File, FuncName, Line, L"General IPC exception") {}

					///<summary>Конструктор помилки</summary>
					///<param name = 'File'>Файл в якому згенерована помилка</param>
					///<param name = 'FunctName'>Ім'я функції</param>
					///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
					///<param name = 'Msg'>Текст помилки</param>
					ForceInline IpcException(const ConstAString& File, const ConstAString& FuncName, SizeT Line, const ConstString& Msg)
						: IoException(File, FuncName, Line, Msg) {}
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
						return ExceptionHolderType(new IpcException(*this));
					}
				public:
					virtual ~IpcException() {}
				};
			}
		}
	}
}