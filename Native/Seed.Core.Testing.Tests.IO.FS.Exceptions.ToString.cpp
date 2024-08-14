#include "Seed.Core.Testing.Tests.IO.FS.Exceptions.ToString.h"
#include "Seed.Core.Threading.h"

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::CantReadMoreDataThanInExistInFileExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::CantReadMoreDataThanInExistInFileException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: Unable to read more than is in file. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::CantWriteDataAfterEofExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::CantWriteDataAfterEofException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: Unable to write data after end of file. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::DirectoryIsNotFileExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::DirectoryIsNotFileException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: Unable to open the directory as a file. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FileBusyExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FileBusyException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: File is busy. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FileExistExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FileExistException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: File already exists. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FileIsCoruptedExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FileIsCoruptedException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: File is corrupted, file system check required. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FileModeNoSupportedByOperationExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FileModeNoSupportedByOperationException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: The operation is not available in the current file mode. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FileNameInvalidExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FileNameInvalidException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: Invalid file name. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FileNotExistExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FileNotExistException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: File not found. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::FsIoExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::FsIoException({ L"PathToFile",300 });
	auto obj2 = Seed::Core::IO::FS::FsIoException({ L"PathToFile",300 }, L"Generic file system I/O exception");
	auto expected = ExceptionString::Combine(L"Generic file system I/O exception. ",
		L"Generated in line 300 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();
	auto actual2 = obj2.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);
	Asserts::AssertEqual(expected, actual2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::GenericFileExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::GenericFileException({ L"PathToFile",400 }, L"SomeFile", 0x00000000L);
	auto obj2 = Seed::Core::IO::FS::GenericFileException({ L"PathToFile",400 }, L"SomeFile", 0x87415610L);
	auto expected = ExceptionString::Combine(
		L"File exception with message: File not found. ",
		L"With message: STATUS_SUCCESS Operation success. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto expected2 = ExceptionString::Combine(
		L"File exception with message: File not found. ",
		L"Unknown status with code: 0x87415610L. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();
	auto actual2 = obj2.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);
	Asserts::AssertEqual(expected2, actual2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::FS::Exceptions::ToString::PathNotFoundExceptionToString::Execute() {
	auto obj = Seed::Core::IO::FS::PathNotFoundException({ L"PathToFile",400 }, L"SomeFile");
	auto expected = ExceptionString::Combine(
		L"File exception with message: Invalid object path. ",
		L"Occurs while processing the file: \"SomeFile\"",
		L"Generated in line 400 in file PathToFile in thread with ID. ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}