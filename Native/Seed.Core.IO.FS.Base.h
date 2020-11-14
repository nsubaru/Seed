#pragma once
#include "Seed.Core.IO.Base.h"

namespace Seed {
	inline namespace Core {
		namespace IO {
			namespace FS {
#pragma region Exceptions
				/// <summary>
				/// Загальна FS I/O помилка
				/// </summary>
				class FsIoException : public IoException {
				public:
					FsIoException(const FsIoException& orig) : IoException(orig) {}
					FsIoException(FsIoException&& from) : IoException(MoveRValue(from)) {}
					FsIoException& operator=(const FsIoException& from) { this->~FsIoException(); this->FsIoException::FsIoException(from); return *this; }
					FsIoException& operator=(FsIoException&& from) { this->~FsIoException(); this->FsIoException::FsIoException(MoveRValue(from)); return *this; }
				public:
					/// <summary>Конструктор помилки</summary>
					/// <param name = 'File'>Файл в якому згенерована помилка</param>
					/// <param name = 'FunctName'>Ім'я функції</param>
					/// <param name = 'Line'>Номер рядка де була згенерована помилка</param>
					ForceInline FsIoException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
						: IoException(File, FuncName, Line, L"General FS I/O exception") {}

					/// <summary>Конструктор помилки</summary>
					/// <param name = 'File'>Файл в якому згенерована помилка</param>
					/// <param name = 'FunctName'>Ім'я функції</param>
					/// <param name = 'Line'>Номер рядка де була згенерована помилка</param>
					/// <param name = 'Msg'>Текст помилки</param>
					ForceInline FsIoException(const ConstAString& File, const ConstAString& FuncName, SizeT Line, const ConstString& Msg)
						: IoException(File, FuncName, Line, Msg) {}
				public:
					/// <summary>Текстове представлення інформації про помилку</summary>
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
						return ExceptionHolderType(new FsIoException(*this));
					}
				public:
					virtual ~FsIoException() {}
				};

				/// <summary>
				/// Загальна файлова помилка
				/// </summary>
				class GenericFileException : public FsIoException {
				private:
					ExceptionString fileName;
				public:
					GenericFileException(const GenericFileException& orig) : FsIoException(orig), fileName(orig.fileName) {}
					GenericFileException(GenericFileException&& from) : FsIoException(MoveRValue(from)), fileName(MoveRValue(from.fileName)) {}
					GenericFileException& operator=(const GenericFileException& from) { this->~GenericFileException(); this->GenericFileException::GenericFileException(from); return *this; }
					GenericFileException& operator=(GenericFileException&& from) { this->~GenericFileException(); this->GenericFileException::GenericFileException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline GenericFileException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName, ConstString&& ErrorName)
						: fileName(fileName), FsIoException(File, FunctName, Line, ErrorName) {}

					virtual ExceptionString toString() const override {
						return ExceptionString::Combine({
							ExceptionString(L"Файлова помилка з повідомленням: "), ExceptionString(exceptionName), ExceptionString(L". "),
							ExceptionString(L"Виникла при обробці файла: \""), fileName, ExceptionString(L"\""),
							location.toString(),
							(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
							});
					}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new GenericFileException(*this));
					}
				public:
					virtual ~GenericFileException() override {}
				};

				/// <summary>
				/// Файл існує
				/// </summary>
				class FileExistException : public GenericFileException {
				public:
					FileExistException(const FileExistException& orig) : GenericFileException(orig) {}
					FileExistException(FileExistException&& from) : GenericFileException(MoveRValue(from)) {}
					FileExistException& operator=(const FileExistException& from) { this->~FileExistException(); this->FileExistException::FileExistException(from); return *this; }
					FileExistException& operator=(FileExistException&& from) { this->~FileExistException(); this->FileExistException::FileExistException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline FileExistException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Файл вже інсує") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new FileExistException(*this));
					}

					virtual ~FileExistException() override {}
				};

				/// <summary>
				/// Файла не існує
				/// </summary>
				class FileNotExistException : public GenericFileException {
				public:
					FileNotExistException(const FileNotExistException& orig) : GenericFileException(orig) {}
					FileNotExistException(FileNotExistException&& from) : GenericFileException(MoveRValue(from)) {}
					FileNotExistException& operator=(const FileNotExistException& from) { this->~FileNotExistException(); this->FileNotExistException::FileNotExistException(from); return *this; }
					FileNotExistException& operator=(FileNotExistException&& from) { this->~FileNotExistException(); this->FileNotExistException::FileNotExistException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline FileNotExistException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Файл не знайдено") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new FileNotExistException(*this));
					}

					virtual ~FileNotExistException() override {}
				};

				/// <summary>
				/// Файла зайннятий іншим процесом
				/// </summary>
				class FileBusyException : public GenericFileException {
				public:
					FileBusyException(const FileBusyException& orig) : GenericFileException(orig) {}
					FileBusyException(FileBusyException&& from) : GenericFileException(MoveRValue(from)) {}
					FileBusyException& operator=(const FileBusyException& from) { this->~FileBusyException(); this->FileBusyException::FileBusyException(from); return *this; }
					FileBusyException& operator=(FileBusyException&& from) { this->~FileBusyException(); this->FileBusyException::FileBusyException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline FileBusyException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Файл зайннятий") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new FileBusyException(*this));
					}

					virtual ~FileBusyException() override {}
				};

				/// <summary>
				/// Спроба відкрити директорію як файл
				/// </summary>
				class DirectoryIsNotFileException : public GenericFileException {
				public:
					DirectoryIsNotFileException(const DirectoryIsNotFileException& orig) : GenericFileException(orig) {}
					DirectoryIsNotFileException(DirectoryIsNotFileException&& from) : GenericFileException(MoveRValue(from)) {}
					DirectoryIsNotFileException& operator=(const DirectoryIsNotFileException& from) { this->~DirectoryIsNotFileException(); this->DirectoryIsNotFileException::DirectoryIsNotFileException(from); return *this; }
					DirectoryIsNotFileException& operator=(DirectoryIsNotFileException&& from) { this->~DirectoryIsNotFileException(); this->DirectoryIsNotFileException::DirectoryIsNotFileException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline DirectoryIsNotFileException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Не можливо відкрити директорію як файл") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new DirectoryIsNotFileException(*this));
					}

					virtual ~DirectoryIsNotFileException() override {}
				};

				/// <summary>
				/// Файл пошкоджений
				/// </summary>
				class FileIsCoruptedException : public GenericFileException {
				public:
					FileIsCoruptedException(const FileIsCoruptedException& orig) : GenericFileException(orig) {}
					FileIsCoruptedException(FileIsCoruptedException&& from) : GenericFileException(MoveRValue(from)) {}
					FileIsCoruptedException& operator=(const FileIsCoruptedException& from) { this->~FileIsCoruptedException(); this->FileIsCoruptedException::FileIsCoruptedException(from); return *this; }
					FileIsCoruptedException& operator=(FileIsCoruptedException&& from) { this->~FileIsCoruptedException(); this->FileIsCoruptedException::FileIsCoruptedException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline FileIsCoruptedException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Файд пошкоджений, потрібна перевірка файлової системи") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new FileIsCoruptedException(*this));
					}

					virtual ~FileIsCoruptedException() override {}
				};

				/// <summary>
				/// Режим відкритя файла не сумісний операцією
				/// </summary>
				class FileModeNoSupportedByOperationException : public GenericFileException {
				public:
					FileModeNoSupportedByOperationException(const FileModeNoSupportedByOperationException& orig) : GenericFileException(orig) {}
					FileModeNoSupportedByOperationException(FileModeNoSupportedByOperationException&& from) : GenericFileException(MoveRValue(from)) {}
					FileModeNoSupportedByOperationException& operator=(const FileModeNoSupportedByOperationException& from) { this->~FileModeNoSupportedByOperationException(); this->FileModeNoSupportedByOperationException::FileModeNoSupportedByOperationException(from); return *this; }
					FileModeNoSupportedByOperationException& operator=(FileModeNoSupportedByOperationException&& from) { this->~FileModeNoSupportedByOperationException(); this->FileModeNoSupportedByOperationException::FileModeNoSupportedByOperationException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline FileModeNoSupportedByOperationException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Операція недоступна в поточному режимі роботи з файлом") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new FileModeNoSupportedByOperationException(*this));
					}

					virtual ~FileModeNoSupportedByOperationException() override {}
				};

				/// <summary>
				/// Спроба считати більше ніж є у файлі
				/// </summary>
				class CantReadMoreDataThanInExistInFileException : public GenericFileException {
				public:
					CantReadMoreDataThanInExistInFileException(const CantReadMoreDataThanInExistInFileException& orig) : GenericFileException(orig) {}
					CantReadMoreDataThanInExistInFileException(CantReadMoreDataThanInExistInFileException&& from) : GenericFileException(MoveRValue(from)) {}
					CantReadMoreDataThanInExistInFileException& operator=(const CantReadMoreDataThanInExistInFileException& from) { this->~CantReadMoreDataThanInExistInFileException(); this->CantReadMoreDataThanInExistInFileException::CantReadMoreDataThanInExistInFileException(from); return *this; }
					CantReadMoreDataThanInExistInFileException& operator=(CantReadMoreDataThanInExistInFileException&& from) { this->~CantReadMoreDataThanInExistInFileException(); this->CantReadMoreDataThanInExistInFileException::CantReadMoreDataThanInExistInFileException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline CantReadMoreDataThanInExistInFileException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Не можливо зчитати з файла більше ніж ньому є") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new CantReadMoreDataThanInExistInFileException(*this));
					}
				};

				/// <summary>
				/// Спроба записати дані після кінця файла
				/// </summary>
				class CantWriteDataAfterEofException : public GenericFileException {
				public:
					CantWriteDataAfterEofException(const CantWriteDataAfterEofException& orig) : GenericFileException(orig) {}
					CantWriteDataAfterEofException(CantWriteDataAfterEofException&& from) : GenericFileException(MoveRValue(from)) {}
					CantWriteDataAfterEofException& operator=(const CantWriteDataAfterEofException& from) { this->~CantWriteDataAfterEofException(); this->CantWriteDataAfterEofException::CantWriteDataAfterEofException(from); return *this; }
					CantWriteDataAfterEofException& operator=(CantWriteDataAfterEofException&& from) { this->~CantWriteDataAfterEofException(); this->CantWriteDataAfterEofException::CantWriteDataAfterEofException(MoveRValue(from)); return *this; }
				public:
					//Генерація помилки
					ForceInline CantWriteDataAfterEofException(ConstAString File, ConstAString FunctName, SizeT Line, ExceptionString&& fileName)
						: GenericFileException(File, FunctName, Line, MoveRValue(fileName), L"Не можливо записати у файл поза його кінцем") {}

					/// <summary>
					/// Вертає копію об'єкта помилки
					/// </summary>
					/// <returns>Розумний покажчик на об'єкт помилки</returns>
					virtual ExceptionHolderType Clone() const override {
						return ExceptionHolderType(new CantWriteDataAfterEofException(*this));
					}

					virtual ~CantWriteDataAfterEofException() override {}
				};
#pragma endregion

			}
		}
	}
}