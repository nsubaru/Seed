#include "Seed.Core.Testing.Tests.Exceptions.ABI.h"

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
			if (testResult)
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
	{
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
	{

	}

	auto result2 = Throw2();
	auto result3 = Throw3();
	auto result4 = Throw5();

	return testResult && result2 && result3 && result4;
}
