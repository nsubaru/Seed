// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com


#include "Seed.Core.Testing.Tests.DI.h"

#include "Seed.Core.Reflection.h"

#include "Seed.Core.DepencyInjection.h"

using namespace Seed;
using namespace Seed::Containers;

class ServiceA{
	int a = 10;
public:
	ServiceA() {};
	void MethodA() {};
	void MethodB(bool State) {};
	bool MethodC(bool State1, bool State2) { return State1 || State2; }

	int GetA() const {
		return a;
	}

	void SetA(int a) {
		this->a = a;
	}

	int Property(GetA, SetA) A;
};

class ServiceB{
	ServiceA service;
	String text;
public:
	ServiceB(const ServiceA & service) : service(service) {}
	void MethodA() {};
	void MethodB(bool State) {};
	bool MethodC(bool State1, bool State2) { return State1 && State2; }
	String MethodD(const List<String> & arg) { return arg.Aggregate([](const String & result, const String& string) { return result + string; }); } 

	String GetText() const {
		return text;
	}

	void SetText(const String& text) {
		this->text = text;
	}

	void SetText(String&& text) {
		this->text = MoveRValue(text);
	}

	SizeT GetTextSize() const {
		return this->text.Lenght;
	}

	String Property(GetText, SetText) Text;
	SizeT Property(GetTextSize) TextSize;
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
		return static_cast<int>(a + b); //-V2564
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
		return static_cast<int>(a + b); //-V2564
	}
};

constexpr auto TestObject1Definition = ObjectDefinition(
	TestObject1,
	CtorDescription(TestObject1),
	ObjectMethodsDefs(
		MethodDescription(TestObject1, Method1),
		MethodDescription(TestObject1, Method2),
		MethodDescription(TestObject1, Method3),
		MethodDescription(TestObject1, Method4)
	),
	ObjectPropsDefs()
);

constexpr auto TestObject2Definition = ObjectDefinition(
	TestObject2,
	CtorDescription(TestObject2, int, int),
	ObjectMethodsDefs(
		MethodDescription(TestObject2, Method1),
		MethodDescription(TestObject2, Method2),
		MethodDescription(TestObject2, Method3),
		MethodDescription(TestObject2, Method4)
	),
	ObjectPropsDefs()
);

bool Seed::Core::Testing::Test::DI::RegisterService::Excecute() {
	auto serviceRecord = Reflection::ObjectRTTIRecord(TestObject1Definition);
	return serviceRecord.ClassName.Lenght != 0;
}

bool Seed::Core::Testing::Test::DI::CallServiceMethodByNameAndArgs::Excecute() {
	auto serviceRecord = Reflection::ObjectRTTIRecord(TestObject1Definition);
	auto methodRecord = serviceRecord.Methods.First([] (const Reflection::ObjectRTTIRecord::MethodRTTIRecord& record) InlineLambda
	{
		return record.Name == L"Method4";
	});
	auto args = Seed::Core::Containers::Array<Seed::Core::Reflection::DynamicObject>(
		{ 
			Seed::Core::Containers::Tuple<int>(10),
			Seed::Core::Containers::Tuple<double>(1.0)
		});

	auto dynObj = serviceRecord.CtorCallerFunction(Seed::Core::Containers::Array<Seed::Core::Reflection::DynamicObject>());

	auto res = methodRecord.MethodCallerFunction(methodRecord.GetMethodPointer(), dynObj, args);

	auto resValue = res.GetValue<int>().GetByIdx<0>();

	serviceRecord.DtorCallerFunction(MoveRValue(dynObj));

	return resValue == 11;
}

bool Seed::Core::Testing::Test::DI::DataTypeInformation::Excecute() {
	auto typeData = Seed::Core::Reflection::DataTypeRecord(
		TypeTraits::TypeTag<Seed::Core::Containers::Tuple<String, Seed::Core::Containers::List<String>>>{});

	return typeData.TypeName.Lenght != 0;
}

bool Seed::Core::Testing::Test::DI::ResolveServiceByDI::Excecute() {
	auto injector = Core::DI::DepencyInjector();

	injector.Register<ServiceA>(
		Reflection::ObjectRTTIRecord(
			ObjectDefinition(
				ServiceA,
				CtorDescription(ServiceA),
				ObjectMethodsDefs(
					MethodDescription(ServiceA, MethodA),
					MethodDescription(ServiceA, MethodB),
					MethodDescription(ServiceA, MethodC)
				),
				ObjectPropsDefs(
					ObjectProperty(ServiceA, A, GetA, 
						MethodDescription(ServiceA, SetA)
					)
				)
			)
		)
	);

	injector.Register<ServiceB, ServiceA>(
		Reflection::ObjectRTTIRecord(
			ObjectDefinition(
				ServiceB,
				CtorDescription(ServiceB, ServiceA),
				ObjectMethodsDefs(
					MethodDescription(ServiceB, MethodA),
					MethodDescription(ServiceB, MethodB),
					MethodDescription(ServiceB, MethodC),
					MethodDescription(ServiceB, MethodD)
				),
				ObjectPropsDefs(
					ObjectProperty(ServiceB, TextSize, GetTextSize),
					ObjectProperty(ServiceB, Text, GetText,
						MethodDescriptionWithArgs(ServiceB, SetText, void, const String&),
						MethodDescriptionWithArgs(ServiceB, SetText, void, String&&)
					)
				)
			)
		)
	);

	auto serviceB = injector.Resolve<ServiceB>();

	return serviceB.MethodC(true, true);
}

#include "Seed.Core.Services.h"

bool Seed::Core::Testing::Test::DI::TestSerilizationService::Excecute() {
	constexpr auto SerilizationServiceDefinition = ObjectDefinition(
		Core::Services::SerilizationService,
		NullCtorDescription(Core::Services::SerilizationService),
		ObjectMethodsDefs(
			MethodDescription(Core::Services::SerilizationService, SerelizeDynObj),
			MethodDescription(Core::Services::SerilizationService, DeserelizeDynObj)
		),
		ObjectPropsDefs()
	);

	auto injector = Core::DI::DepencyInjector();
	injector.Register<Core::Services::SerilizationService, Core::DI::IDI>
			(SerilizationServiceDefinition);

	auto serilizationService = injector.Resolve<Core::Services::SerilizationService>();

	auto dynamic = Dynamic(10);

	auto serilizedDynamic = serilizationService.Serelize(dynamic);

	auto dynamicDeserilized = serilizationService.Deserelize<Dynamic>(serilizedDynamic);

	Asserts::AssertEqual(
		dynamic.GetVariables<int>().GetByIdx<0>(), 
		dynamicDeserilized.GetVariables<int>().GetByIdx<0>());

	return true;
}
