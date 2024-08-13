#include "Seed.Core.Testing.Tests.Exceptions.ToString.h"
#include "Seed.Core.Threading.h"

bool Seed::Core::Testing::Test::Exceptions::ToString::AlreadyExistExceptionToString::Execute() {
	auto obj = AlreadyExistException({L"PathToFile",400 });
	auto obj2 = AlreadyExistException({L"PathToFile",400 },L"Already exists");
	auto expected = ExceptionString::Combine(L"Already exists. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();
	auto actual2 = obj2.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);
	Asserts::AssertEqual(expected, actual2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::BadCastToString::Execute() {
	auto obj = BadCast({ L"PathToFile",300 });
	auto expected = ExceptionString::Combine(L"Impossible to transform the type. ",
		L"Generated in line 300 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::BadExceptionToString::Execute() {
	auto obj = BadException({ L"PathToFile",200 });
	auto expected = ExceptionString::Combine(L"Invalid exception type. ",
		L"Generated in line 200 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::BadPureCallToString::Execute() {
	auto obj = BadPureCall({ L"PathToFile",700 });
	auto expected = ExceptionString::Combine(L"Impossible to call the virtual function. ",
		L"Generated in line 700 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::DivideByZeroToString::Execute() {
	auto obj = DivideByZero({ L"PathToFile",600 });
	auto expected = ExceptionString::Combine(L"The division by zero. ",
		L"Generated in line 600 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::EmptyQueueExceptionToString::Execute() {
	auto obj = EmptyQueueException({ L"PathToFile",500 });
	auto expected = ExceptionString::Combine(L"Trying to remove items from an empty queue. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::EmptyStackExceptionToString::Execute() {
	auto obj = EmptyStackException({ L"PathToFile",500 });
	auto expected = ExceptionString::Combine(L"Trying to remove items from an empty stack. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::EndNodeNotReferenceableToString::Execute() {
	auto obj = EndNodeNotReferenceable({ L"PathToFile", 500 });
	auto expected = ExceptionString::Combine(L"Not definned iterator at the end. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::FullQueueExceptionToString::Execute() {
	auto obj = FullQueueException({ L"PathToFile",500 }, 50);
	auto expected = ExceptionString::Combine(L"Maximal queue size exceeded. ",
		L"Maximal allowed size - 50. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::FullStackExceptionToString::Execute() {
	auto obj = FullStackException({ L"PathToFile",500 }, 50);
	auto expected = ExceptionString::Combine(L"Maximum stack size is exceeded. ",
		L"Maximal allowed size - 50. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::IndexOutOfRangeToString::Execute() {
	auto obj = IndexOutOfRange({ L"PathToFile", 500 }, 61, 59);
	auto expected = ExceptionString::Combine(L"Out of range. ",
		L"During the try to access the element by index 61 if maximal allowed index 59. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::IntegerOverflowExceptionToString::Execute() {
	auto obj = IntegerOverflowException({ L"PathToFile", 500 });
	auto expected = ExceptionString::Combine(L"Integer overflow. ",
		L"Generated in line 500 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::InternalErrorToString::Execute() {
	auto obj = InternalError({ L"PathToFile",100 });
	auto expected = ExceptionString::Combine(
		L"Kernel internal exception. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::InteruptExceptionToString::Execute() {
	auto obj = InteruptException({ L"PathToFile",100 });
	auto expected = ExceptionString::Combine(
		L"The operation is interrupted. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::InvalidArgumentExceptionToString::Execute() {
	auto obj = InvalidArgumentException({ L"PathToFile",100 }, L"Argument", L"Additional information");
	auto expected = ExceptionString::Combine(
		L"Invalid argument. ",
		L"Invalid Argument",
		L" argument. Additional description: Additional information. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::InvalidFormatExceptionToString::Execute() {
	auto obj = InvalidFormatException({ L"PathToFile",100 });
	auto expected = ExceptionString::Combine(
		L"Invalid format. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::InvalidHandleUsageToString::Execute() {
	auto obj = InvalidFormatException({ L"PathToFile",100 });
	auto expected = ExceptionString::Combine(
		L"Using the invalid descriptor. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::InvalidTypeExceptionToString::Execute() {
	auto obj = InvalidTypeException({ L"PathToFile",100 }, typeid(String), typeid(Int16));
	auto expected = ExceptionString::Combine(
		L"Wrong type. ",
		L"The expected type: String. The actual type: Int16. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::NotFoundExceptionToString::Execute() {
	auto obj = NotFoundException({ L"PathToFile",100 });
	auto expected = ExceptionString::Combine(
		L"Not found. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::NotImplementedExceptionToString::Execute() {
	auto obj = NotImplementedException({ L"PathToFile",100 });
	auto obj2 = NotImplementedException({ L"PathToFile",100 }, L"Function\\Method is not implemented");
	auto expected = ExceptionString::Combine(
		L"Function\\Method is not implemented. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();
	auto actual2 = obj2.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);
	Asserts::AssertEqual(expected, actual2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::NtExceptionToString::Execute() {
	auto obj = NtException({ L"PathToFile",100 }, 0x00000000L);
	auto expected = ExceptionString::Combine(
		L"NT system call exception. ",
		L"The system call returns the code ",
		L"0x00000000L", L". Msg: ", L"STATUS_SUCCESS ",
		L"Operation success",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::NullPointExceptionToString::Execute() {
	auto obj = NullPointException({ L"PathToFile",100 }, 0x14, Seed::Core::NullPointException::Types::Read);
	auto expected = ExceptionString::Combine(
		L"The memory access error for the address 0x14 when trying read. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::OutOfMemoryToString::Execute() {
	auto obj = OutOfMemory({ L"PathToFile",100 }, L"MainHeap", 15, 22, 10);
	auto obj2 = OutOfMemory({ L"PathToFile",100 }, L"MainHeap", 15, 22, 1);
	auto expected = ExceptionString::Combine(
		L"Not enough RAM for the operation in MainHeap heap during the allocation 15 bytes,",
		"at the moment of exception occurring was allocated 22 ",
		"bytes in 10 memory blocks. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto expected2 = ExceptionString::Combine(
		L"Not enough RAM for the operation in MainHeap heap during the allocation 15 bytes,",
		"at the moment of exception occurring was allocated 22 ",
		"bytes in 10 memory block. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();
	auto actual2 = obj2.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);
	Asserts::AssertEqual(expected2, actual2, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::StackOverflowExceptionToString::Execute() {
	auto obj = StackOverflowException({ L"PathToFile",100 });
	auto expected = ExceptionString::Combine(
		L"Stack overflow. ",
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::ToString::StringToLongToString::Execute() {
	auto obj = StringToLong({ L"PathToFile",100 }, 60);
	auto expected = ExceptionString::Combine(
		L"String is too long. ", L"Requested size 60. "
		L"Generated in line 100 in file PathToFile in thread with ID ",
		Threading::GetTID());
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}