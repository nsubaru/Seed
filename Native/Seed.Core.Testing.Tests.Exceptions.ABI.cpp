// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Exceptions.Abi.h"

#include "Seed.Core.Threading.h"

using namespace Seed;

class TestException : public Exception {
private:
	Int32 statusCode;
public:
	TestException(const TestException& orig) : Exception(orig), statusCode(orig.statusCode) {}
	TestException(TestException&& from) : Exception(MoveRValue(from)), statusCode(from.statusCode) {}
	TestException& operator=(const TestException& from) { this->~TestException(); this->TestException::TestException(from); return *this; }
	TestException& operator=(TestException&& from) { this->~TestException(); this->TestException::TestException(MoveRValue(from)); return *this; }
public:
	TestException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, Int32 StatusCode)
		noexcept : Exception(File, FunctName, Line, L"Помилка системного виклику NT"), statusCode(StatusCode)
	{}
public:
	Int32 GetCode() const { return statusCode; }

	Int32 Property(GetCode) Code;
public:
	virtual ExceptionString toString() const override {
		return ExceptionString::Combine({
			ExceptionString(exceptionName),
			ExceptionString(L". Тестова помилка з кодом "),
			BaseString<Memory::ExceptionHeap>::Int32asHex(statusCode), ExceptionString(L". "),
			location.toString(),
			(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
			});
	}

	virtual ExceptionHolderType Clone() const override {
		return ExceptionHolderType(new TestException(*this));
	}

	virtual ~TestException() {}
};


static __declspec(noinline) void ThrowExceptionFunct() {
	String str = L"123";
	Throw(TestException(ExceptionPosition, 1));
}

static __declspec(noinline) bool ThrowSubMethodFunct() {
	try
	{
		String str = L"123";
		ThrowExceptionFunct();
	}
	catch (TestException & ex)
	{
		if (ex.Location.File == String(__FILE__) && ex.Location.FunctName != String(__FUNCSIG__) && ex.Code == 1)
			return true;
		else
			return false;
	}
	return false;
}

static __declspec(noinline) bool ThrowSubMethodElipseFunct() {
	try
	{
		String str = L"123";
		ThrowExceptionFunct();
	}
	catch (...)
	{
		return true;
	}
	return false;
}

static __declspec(noinline) void ThrowMethodWithoutExceptionFunct() {
	String str = L"123";
	ThrowExceptionFunct();
}

static __declspec(noinline) bool ThrowCatchSubSubMethodFunct() {
	try
	{
		String str = L"123";
		ThrowMethodWithoutExceptionFunct();
	}
	catch (...)
	{
		return true;
	}
	return false;
}

class TestDtorException {
public:
	String text;
	__declspec(noinline) TestDtorException(String&& text) : text(MoveRValue(text)) {}
	__declspec(noinline) ~TestDtorException() {
		Throw(TestException(ExceptionPosition, 8));
	}
};

static __declspec(noinline) bool ThrowDtorGenInnerExceptionFunct() {
	try
	{
		TestDtorException obj(L"123"S);
		ThrowMethodWithoutExceptionFunct();
	}
	catch (TestException& ex)
	{
		return ex.Code == 1 && ex.InnerException != nullptr && ex.InnerException.As<TestException>().Code == 8;
	}
	return false;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Transfering::StoringException::Excecute() {
	try
	{
		Threading::Thread(L"Потік тестування передачі помилки", [] ()
		{
			try
			{
				ThrowExceptionFunct();
			}
			catch (...)
			{
				return Seed::Exceptions::Exception::StoreCurrentException();
			}

			Throw(TestException(ExceptionPosition, 7));
		})
			.Start()
			.Join()
			.GetThreadResult<Seed::Exceptions::Exception::RawException>().ThrowThis();
	}
	catch (TestException & ex)
	{
		(void) (ex);
		return true;
	}
	return false;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::SimpleThrow::Excecute()
{
	bool testResult = false;

	try
	{
		Throw(TestException(ExceptionPosition, 2));
	}
	catch (TestException& ex)
	{
		if (ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__) &&
			ex.Code == 2
			) {
			testResult = true;
		}
	}

	Asserts::AssertTrue(testResult);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::SimpleThrowWithTwoCatches::Excecute()
{
	bool testResult = false;

	try
	{
		Throw(TestException(ExceptionPosition, 3));
	}
	catch (UnusedData NotImplementedException& ex)
	{
		testResult = false;
	}
	catch (TestException& ex)
	{
		if (ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__) &&
			ex.Code == 3
			) {
			testResult = true;
		}
	}

	Asserts::AssertTrue(testResult);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::SimpleThrowCatchAsParrentType::Excecute()
{
	bool testResult = true;

	try
	{
		Throw(TestException(ExceptionPosition, 4));
	}
	catch (Exception& ex)
	{
		if (!(ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__))
			) {
			testResult = false;
		}
	}

	Asserts::AssertTrue(testResult);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::SimpleThrowElipseCatch::Excecute()
{
	bool testResult = false;

	try
	{
		Throw(TestException(ExceptionPosition, 5));
	}
	catch (...)
	{
		testResult = true;
	}

	Asserts::AssertTrue(testResult);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::SimpleThrowInnerCatch::Excecute()
{
	bool innerCacth = false;
	bool outerCatch = false;

	try
	{
		try
		{
			Throw(TestException(ExceptionPosition, 6));
		}
		catch (TestException& ex)
		{
			if (
				ex.Location.File == String(__FILE__) &&
				ex.Location.FunctName == String(__FUNCSIG__) &&
				ex.Code == 6
				) {
				innerCacth = true;
			}

			ReThrow;
		}
	}
	catch (...)
	{
		if (innerCacth == true)
			outerCatch = true;
	}

	Asserts::AssertTrue(innerCacth);
	Asserts::AssertTrue(outerCatch);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::ThrowSubMethod::Excecute()
{
	auto result = ThrowSubMethodFunct();

	Asserts::AssertTrue(result);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::ThrowSubMethodElipse::Excecute()
{
	auto result = ThrowSubMethodElipseFunct();

	Asserts::AssertTrue(result);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::ThrowCatchSubSubMethod::Excecute()
{
	auto result = ThrowCatchSubSubMethodFunct();

	Asserts::AssertTrue(result);

	return true;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Throwing::ThrowDtorGenInnerException::Excecute()
{
	auto result = ThrowDtorGenInnerExceptionFunct();

	Asserts::AssertTrue(result);

	return true;
}
