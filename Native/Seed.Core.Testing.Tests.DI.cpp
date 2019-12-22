// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include "Seed.Core.Testing.Tests.DI.h"

#include "Seed.Core.Reflection.h"

#include "Seed.Core.DepencyInjection.h"

using namespace Seed;
using namespace Seed::Containers;

class ServiceA{
public:
	ServiceA() {};
	void MethodA() {};
	void MethodB(bool State) {};
	bool MethodC(bool State1, bool State2) { return State1 || State2; }
};

class ServiceB{
	ServiceA service;
public:
	ServiceB(const ServiceA & service) : service(service) {}
	void MethodA() {};
	void MethodB(bool State) {};
	bool MethodC(bool State1, bool State2) { return State1 && State2; }
	String MethodD(const List<String> & arg) { return arg.Aggregate([](String & result, const String& string) { result += string; }); } 
};

class TestObject1 {
public:
	TestObject1() {};
	void Method1() {};
	void Method2(int) {};
	int Method3(int a) {
		return a;
	}
	int Method4(int a, double b) {
		return static_cast<int>(a + b);
	}
};

class TestObject2 {
public:
	TestObject2(int a, int b) {};
	void Method1() {};
	void Method2(int) {};
	int Method3(int a) {
		return a;
	}
	int Method4(int a, double b) {
		return static_cast<int>(a + b);
	}
};

using TestObject1Definition = ObjectDefinition(
	TestObject1,
	CtorDescription(TestObject1),
	MethodDescription(TestObject1, Method1),
	MethodDescription(TestObject1, Method2),
	MethodDescription(TestObject1, Method3),
	MethodDescription(TestObject1, Method4)
);

using TestObject2Definition = ObjectDefinition(
	TestObject2,
	CtorDescription(TestObject2, int, int),
	MethodDescription(TestObject2, Method1),
	MethodDescription(TestObject2, Method2),
	MethodDescription(TestObject2, Method3),
	MethodDescription(TestObject2, Method4)
);

bool Seed::Core::Testing::Test::DI::RegisterService::Excecute() {
	auto serviceRecord = Reflection::ObjectRTTIRecord(TypeTraits::TypeTag<TestObject1Definition>{});
	return serviceRecord.ClassName.Size != 0;
}

bool Seed::Core::Testing::Test::DI::CallServiceMethodByNameAndArgs::Excecute() {
	auto serviceRecord = Reflection::ObjectRTTIRecord(TypeTraits::TypeTag<TestObject1Definition>{});
	auto methodRecord = serviceRecord.Methods.First([] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& record)
	{
		return record.Name == L"Method4";
	});
	auto args = Reflection::DynamicObject(Containers::Tuple<int, double>(10, 1.0));
	auto dynObj = serviceRecord.CtorCallerFunction(Reflection::DynamicObject(Containers::Tuple<>()));
	auto res = methodRecord.MethodCallerFunction(methodRecord.GetMethodPointer(), dynObj, args);
	auto resValue = res.GetValue<int>().GetByIdx<0>();
	serviceRecord.DtorCallerFunction(dynObj);
	return resValue == 11;
}

bool Seed::Core::Testing::Test::DI::DataTypeInformation::Excecute() {
	auto typeData = Reflection::DataTypeRecord(TypeTraits::TypeTag<Containers::Tuple<String, Containers::List<String>>>{});
	return typeData.TypeName.Size != 0;
}

bool Seed::Core::Testing::Test::DI::ResolveServiceByDI::Excecute() {
	auto injector = Core::DI::DepencyInjector();

	injector.Register<ServiceA>(
		Reflection::ObjectRTTIRecord(TypeTraits::TypeTag<
		ObjectDefinition(
			ServiceA,
			CtorDescription(ServiceA),
			MethodDescription(ServiceA, MethodA),
			MethodDescription(ServiceA, MethodB),
			MethodDescription(ServiceA, MethodC)
		)> {}
		)
		);

	injector.Register<ServiceB, ServiceA>(
		Reflection::ObjectRTTIRecord(TypeTraits::TypeTag<
		ObjectDefinition(
			ServiceB,
			CtorDescription(ServiceB,ServiceA),
			MethodDescription(ServiceB, MethodA),
			MethodDescription(ServiceB, MethodB),
			MethodDescription(ServiceB, MethodC),
			MethodDescription(ServiceB, MethodD)
		)> {}
		)
		);

	auto serviceB = injector.Resolve<ServiceB>();

	return serviceB.MethodC(true, true);
}
