// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Exceptions.ABI.h"

#include "Seed.Core.Threading.h"

using namespace Seed;

void Throw1() {
	String str = L"123";
	Throw(NtException(ExceptionPosition, 0));
}

bool Throw2() {
	try
	{
		String str = L"123";
		Throw1();
	}
	catch (NtException & ex)
	{
		if (ex.Location.File == String(__FILE__) && ex.Location.FunctName != String(__FUNCSIG__))
			return true;
		else
			return false;
	}
	return false;
}

bool Throw3() {
	try
	{
		String str = L"123";
		Throw1();
	}
	catch (...)
	{
		return true;
	}
	return false;
}

void Throw4() {
	String str = L"123";
	Throw1();
}

bool Throw5() {
	try
	{
		String str = L"123";
		Throw4();
	}
	catch (...)
	{
		return true;
	}
	return false;
}

bool Seed::Core::Testing::Test::Exceptions::Abi::Excecute() {
	bool testResult = true;

	try
	{
		Throw(NtException(ExceptionPosition, 0));
	}
	catch (NtException & ex)
	{
		if (!(ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__)))
			if (testResult) //-V547
				testResult = false;
	}

	try
	{
		Throw(NtException(ExceptionPosition, 0));
	}
	catch (NotImplemented & ex)
	{
		if (!(ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__)))
			if (testResult)
				testResult = false;
	}
	catch (NtException & ex)
	{
		if (!(ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__)))
			if (testResult)
				testResult = false;
	}

	try
	{
		Throw(NtException(ExceptionPosition, 0));
	}
	catch (Exception & ex)
	{
		if (!(ex.Location.File == String(__FILE__) &&
			ex.Location.FunctName == String(__FUNCSIG__)))
			if (testResult)
				testResult = false;
	}

	try
	{
		Throw(NtException(ExceptionPosition, 0));
	}
	catch (...)
	{ //-V565
		;
	}

	try
	{
		try
		{
			Throw(NtException(ExceptionPosition, 0));
		}
		catch (NtException& ex)
		{
			if (!(ex.Location.File == String(__FILE__) &&
				ex.Location.FunctName == String(__FUNCSIG__)))
				if (testResult)
					testResult = false;
			ReThrow;
		}
	}
	catch (...)
	{ //-V565
		;
	}

	auto result2 = Throw2();
	auto result3 = Throw3();
	auto result4 = Throw5();

	return testResult && result2 && result3 && result4;
}

bool Seed::Core::Testing::Test::Exceptions::StoringException::Excecute() {
	try
	{
		Threading::Thread(L"Потік тестування передачі помилки", [] ()
		{
			try
			{
				Throw1();
			}
			catch (...)
			{
				return Seed::Exceptions::Exception::StoreCurrentException();
			}
			Throw(NullPointException(ExceptionPosition));
		})
			.Start()
			.Join()
			.GetThreadResult<Seed::Exceptions::Exception::RawException>().ThrowThis();
	}
	catch (NtException & ex)
	{
		(void) (ex);
		return true;
	}
	return false;
}
