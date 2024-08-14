#include "Seed.Core.Testing.Tests.IO.Network.Exceptions.ToString.h"
#include "Seed.Core.Threading.h"

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::AddressAlreadyBusyExceptionToString::Execute(){
	auto obj = Seed::Core::IO::Network::AddressAlreadyBusyException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The specified address or port is already taken. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::ConnectionAlreadyCloseExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::ConnectionAlreadyCloseException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"Connection is already stopped. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::ConnectionClosedBadExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::ConnectionClosedBadException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The connection was terminated incorrectly. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::InvalidAddressExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::InvalidAddressException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The specified address or port is invalid. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::InvalidAddressFamilyExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::InvalidAddressFamilyException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The specified address or port is invalid. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::InvalidDriverArgumentExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::InvalidDriverArgumentException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The driver failed due to an error in the argument. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::InvalidNetworkFamilyExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::InvalidNetworkFamilyException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The specified network type is invalid. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::InvalidStateExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::InvalidStateException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"Invalid object state. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::IOTimeoutExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::IOTimeoutException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The specified action was not performed in time. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::NetworkUnreachableExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::NetworkUnreachableException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The driver failed due to an error in the argument. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}

bool Seed::Core::Testing::Test::IO::Network::Exceptions::ToString::SoketExceptionToString::Execute() {
	auto obj = Seed::Core::IO::Network::SoketException({ L"PathToFile",400 });
	auto expected = ExceptionString::Combine(
		L"The exception working with sockets. ",
		L"Generated in line 400 in file PathToFile in thread with ID ",
		Threading::GetTID(), L".");
	auto actual = obj.toString();

	Asserts::AssertEqual(expected, actual, SourcePosition);

	return true;
}