#pragma once
#include "Seed.Core.IO.FS.Base.h"
#include "Seed.Core.IO.FS.File.h"
#include "Seed.Core.IO.FS.AsyncFile.h"

namespace Seed::Core::IO::FS::Writers
{
	/// <summary>
	/// Writer interface
	/// </summary>
	/// <typeparam name="T">Writer type</typeparam>
	template<typename T>
	VirtualClass IWriter
	{
	public:
		virtual void Write(const T& record) = 0;

	public:
		virtual ~IWriter() {}
	};


	/// <summary>
	/// Async writer interface
	/// </summary>
	/// <typeparam name="T">Writer type</typeparam>
	template<typename T>
	VirtualClass IAsyncWriter
	{
	public:
		virtual Threading::Task<void> AsyncWrite(const T& record) = 0;

	public:
		virtual ~IAsyncWriter() {}
	};

	/// <summary>
	/// Writer type concept
	/// </summary>
	template<typename TWriter, typename T>
	concept Writer = requires () { TWriter::IsTextMode; };

	/// <summary>
	/// Async writer type concept
	/// </summary>
	template<typename TWriter, typename T>
	concept AsyncWriter = Concepts::IsBasedOn<IAsyncWriter<T>, TWriter> && 
		requires () { TWriter::IsTextMode; };

	/// <summary>
	/// Base for all writer
	/// </summary>
	/// <typeparam name="T">Writer record type</typeparam>
	/// <typeparam name="TWriter">Writer type</typeparam>
	template<typename T, typename TWriter>
	class BaseWriter : public IDisposable, public IWriter<T> {
	protected:
		File file;

	protected:
		BaseWriter(File&& file) : file(MoveRValue(file)) {}
		BaseWriter(const BaseWriter& writer) : file(writer.file) {}
		BaseWriter(BaseWriter&& writer) : file(MoveRValue(writer.file)) {}

	public:
		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static TWriter CreateFile(IO::FS::FilePath&& path, bool overrideIfExist = false)
		{
			auto file = RawCreateFile(path.Value, EOpeningMode::Write, overrideIfExist);

			if constexpr (TWriter::IsTextMode)
			{
				file.Write(Utf16Marker);
			}

			return TWriter(MoveRValue(file));
		}

		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static TWriter CreateFile(const IO::FS::FilePath& path, bool overrideIfExist = false)
		{
			auto file = RawCreateFile(path.Value, EOpeningMode::Write, overrideIfExist);

			if constexpr (TWriter::IsTextMode)
			{
				file.Write(Utf16Marker);
			}

			return TWriter(MoveRValue(file));
		}

	public:
		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static TWriter CreateFile(String&& path, bool overrideIfExist = false)
		{
			auto file = RawCreateFile(path, EOpeningMode::Write, overrideIfExist);

			if constexpr (TWriter::IsTextMode)
			{
				file.Write(Utf16Marker);
			}

			return TWriter(MoveRValue(file));
		}

		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static TWriter CreateFile(const String& path, bool overrideIfExist = false)
		{
			auto file = RawCreateFile(path, EOpeningMode::Write, overrideIfExist);

			if constexpr (TWriter::IsTextMode)
			{
				file.Write(Utf16Marker);
			}

			return TWriter(MoveRValue(file));
		}
	};

	/// <summary>
	/// Base for all writer
	/// </summary>
	/// <typeparam name="T">Writer record type</typeparam>
	/// <typeparam name="TWriter">Writer type</typeparam>
	template<typename T, typename TWriter>
	class BaseAsyncWriter : public IAsyncWriter<T>, public IDisposable {
	private:
		struct BaseData {
			AsyncFile file;
			SizeT position;
			Threading::SRWLock lock;
			RefCountT RefCount;

			BaseData(AsyncFile&& file) : file(MoveRValue(file)), position(0), lock(), RefCount(1) {}
		};

	private:
		BaseData* BaseThis;

	protected:
		virtual void Dispose() = 0 {}

	protected:
		BaseAsyncWriter(AsyncFile&& file) : BaseThis(new BaseData(MoveRValue(file))) {}

	protected:
		virtual Threading::Task<void> Init()
		{
			auto task = Threading::Task<void>();
			task.MarkFinished();
			return task;
		}

	public:
		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static Threading::Task<TWriter> CreateAsyncFile(IO::FS::FilePath&& path, bool overrideIfExist = false)
		{
			auto file = AsyncFile(MoveRValue(path), EOpeningMode::Write, overrideIfExist ? OpenDisposition::OverwriteIf : OpenDisposition::Create);

			auto position = 0Sz;
			if constexpr (TWriter::IsTextMode)
			{
				co_await file.AsyncWrite(Utf16Marker, 0);
				position = sizeof(Utf16Marker);
			}

			auto writer = TWriter(MoveRValue(file));
			static_cast<BaseAsyncWriter<T, TWriter>&>(writer).This->position = position;

			co_await static_cast<BaseAsyncWriter<T, TWriter>&>(writer).Init();

			co_return writer;
		}

		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static Threading::Task<TWriter> CreateAsyncFile(const IO::FS::FilePath& path, bool overrideIfExist = false)
		{
			auto file = AsyncFile(path, EOpeningMode::Write, overrideIfExist ? OpenDisposition::OverwriteIf : OpenDisposition::Create);

			auto position = 0Sz;
			if constexpr (TWriter::IsTextMode)
			{
				co_await file.AsyncWrite(Utf16Marker, 0);
				position = sizeof(Utf16Marker);
			}

			auto writer = TWriter(MoveRValue(file));
			static_cast<BaseAsyncWriter<T, TWriter>&>(writer).This->position = position;

			co_await static_cast<BaseAsyncWriter<T, TWriter>&>(writer).Init();

			co_return writer;
		}

	public:
		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static Threading::Task<TWriter> CreateAsyncFile(String&& path, bool overrideIfExist = false)
		{
			auto file = AsyncFile(MoveRValue(path), EOpeningMode::Write, overrideIfExist ? OpenDisposition::OverwriteIf : OpenDisposition::Create);

			auto position = 0Sz;
			if constexpr (TWriter::IsTextMode)
			{
				co_await file.AsyncWrite(Utf16Marker, 0);
				position = sizeof(Utf16Marker);
			}

			auto writer = TWriter(MoveRValue(file));
			static_cast<BaseAsyncWriter<T, TWriter>&>(writer).This->position = position;

			co_await static_cast<BaseAsyncWriter<T, TWriter>&>(writer).Init();

			co_return writer;
		}

		/// <summary>
		/// Create writer file
		/// </summary>
		/// <param name="path">File path</param>
		/// <param name="overrideIfExist">If true, then override file if exist</param>
		/// <returns><Writer/returns>
		static Threading::Task<TWriter> CreateAsyncFile(const String& path, bool overrideIfExist = false)
		{
			auto file = AsyncFile(FilePath(path), EOpeningMode::Write, overrideIfExist ? OpenDisposition::OverwriteIf : OpenDisposition::Create);

			auto position = 0Sz;
			if constexpr (TWriter::IsTextMode)
			{
				co_await file.AsyncWrite(Utf16Marker, 0);
				position = sizeof(Utf16Marker);
			}

			auto writer = TWriter(MoveRValue(file));
			static_cast<BaseAsyncWriter<T, TWriter>&>(writer).This->position = position;

			co_await static_cast<BaseAsyncWriter<T, TWriter>&>(writer).Init();

			co_return writer;
		}

	protected:
		template<Concepts::IsInvocableReturnSameAs<String> TLambda>
		Threading::Task<String> AsyncWriteToFile(TLambda&& dataGen)
		{
			return BaseThis->lock.ExclusiveLockAsync([&] () -> Threading::Task<String> {
				String data = dataGen();
				auto dataLenght = data.Length * 2;
				String result = co_await BaseThis->file.AsyncWrite(MoveRValue(data), BaseThis->position);

				BaseThis->position += dataLenght;

				co_return result;
			});
		}

	public:
		RefCountInterface(BaseAsyncWriter, BaseThis);
	};
}