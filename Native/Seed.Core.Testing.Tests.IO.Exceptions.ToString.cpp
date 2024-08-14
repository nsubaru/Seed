#include "Seed.Core.Testing.Tests.IO.Exceptions.ToString.h"
#include "Seed.Core.Threading.h"

bool Seed::Core::Testing::Test::IO::Exceptions::ToString::IoExceptionToString::Execute (){
	auto obj = Seed::Core::IO::IoException({L"PathToFile",400 });
	auto obj2 = Seed::Core::IO::IoException({L"PathToFile",400 }, L"General I/O exception");
	auto expected = ExceptionString::Combine(L"General I/O exception. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L" ");
	auto actual = obj.toString();
	auto actual2 = obj2.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);
	Asserts::AssertEqual(expected, actual2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Exceptions::ToString::OperationCancelledExceptionToString::Execute (){
	auto obj = Seed::Core::IO::OperationCancelledException({L"PathToFile",400 });
	auto expected = ExceptionString::Combine(L"Operation was cancelled. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L" ");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Exceptions::ToString::OperationTimeoutExceptionToString::Execute() {
	auto obj = Seed::Core::IO::OperationTimeoutException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(L"Operation was cancelled by timeout. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L" ");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}