#pragma once
#include "Seed.Core.IO.IPC.Base.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace IPC {
				namespace Pipes {
					/// <summary>
					/// Пайп відключився з другої стороеи
					/// </summary>
					class PipeBrokenException : public Exception {
					public:
						PipeBrokenException(const PipeBrokenException& orig) : Exception(orig) {}
						PipeBrokenException(PipeBrokenException&& from) : Exception(MoveRValue(from)) {}
						PipeBrokenException& operator=(const PipeBrokenException& from) { this->~PipeBrokenException(); this->PipeBrokenException::PipeBrokenException(from); return *this; }
						PipeBrokenException& operator=(PipeBrokenException&& from) { this->~PipeBrokenException(); this->PipeBrokenException::PipeBrokenException(MoveRValue(from)); return *this; }
					public:
						///<summary>Конструктор помилки</summary>
						///<param name = 'File'>Файл в якому згенерована помилка</param>
						///<param name = 'FunctName'>Ім'я функції</param>
						///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
						ForceInline PipeBrokenException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
							: Exception(File, FuncName, Line, L"Pipe transport broken") {}
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
							return ExceptionHolderType(new PipeBrokenException(*this));
						}
					public:
						virtual ~PipeBrokenException() {}
					};

					/// <summary>
					/// Невалідний пайп
					/// </summary>
					class InvalidPipeException : public Exception {
					public:
						InvalidPipeException(const InvalidPipeException& orig) : Exception(orig) {}
						InvalidPipeException(InvalidPipeException&& from) : Exception(MoveRValue(from)) {}
						InvalidPipeException& operator=(const InvalidPipeException& from) { this->~InvalidPipeException(); this->InvalidPipeException::InvalidPipeException(from); return *this; }
						InvalidPipeException& operator=(InvalidPipeException&& from) { this->~InvalidPipeException(); this->InvalidPipeException::InvalidPipeException(MoveRValue(from)); return *this; }
					public:
						///<summary>Конструктор помилки</summary>
						///<param name = 'File'>Файл в якому згенерована помилка</param>
						///<param name = 'FunctName'>Ім'я функції</param>
						///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
						ForceInline InvalidPipeException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
							: Exception(File, FuncName, Line, L"Pipe transport invalid") {}
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
							return ExceptionHolderType(new InvalidPipeException(*this));
						}
					public:
						virtual ~InvalidPipeException() {}
					};

					/// <summary>
					/// Невалідний режим пайпа
					/// </summary>
					class InvalidPipeModeException : public Exception {
					public:
						InvalidPipeModeException(const InvalidPipeModeException& orig) : Exception(orig) {}
						InvalidPipeModeException(InvalidPipeModeException&& from) : Exception(MoveRValue(from)) {}
						InvalidPipeModeException& operator=(const InvalidPipeModeException& from) { this->~InvalidPipeModeException(); this->InvalidPipeModeException::InvalidPipeModeException(from); return *this; }
						InvalidPipeModeException& operator=(InvalidPipeModeException&& from) { this->~InvalidPipeModeException(); this->InvalidPipeModeException::InvalidPipeModeException(MoveRValue(from)); return *this; }
					public:
						///<summary>Конструктор помилки</summary>
						///<param name = 'File'>Файл в якому згенерована помилка</param>
						///<param name = 'FunctName'>Ім'я функції</param>
						///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
						ForceInline InvalidPipeModeException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
							: Exception(File, FuncName, Line, L"Pipe transport mode invalid") {}
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
							return ExceptionHolderType(new InvalidPipeModeException(*this));
						}
					public:
						virtual ~InvalidPipeModeException() {}
					};
				}
			}
		}
	}
}