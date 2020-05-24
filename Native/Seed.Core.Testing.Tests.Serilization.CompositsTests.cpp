// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Serilization.CompositsTests.h"

bool Seed::Core::Testing::Test::Serilization::SimpleTypeTupleSerilization::Excecute() {
	auto tuple = Seed::Core::Containers::Tuple<Int32, Int64, Char, Boolean>{1,1,'1',true};

	auto stream = Core::Serilization::Serilize(tuple);
	decltype(tuple) deserelizedTuple;

	Core::Serilization::Deserilize(stream, deserelizedTuple);

	return 
		tuple.GetByIdx<0>() == deserelizedTuple.GetByIdx<0>() &&
		tuple.GetByIdx<1>() == deserelizedTuple.GetByIdx<1>() &&
		tuple.GetByIdx<2>() == deserelizedTuple.GetByIdx<2>() &&
		tuple.GetByIdx<3>() == deserelizedTuple.GetByIdx<3>();
}

bool Seed::Core::Testing::Test::Serilization::ComplexTypeTupleSerilization::Excecute() {
	auto tuple = Seed::Core::Containers::Tuple<String, Core::Containers::Array<Int32>, Core::Containers::List<Int32>, Boolean>
				{L"test",{1,2,3,4},{1,2,3,4},true};

	auto stream = Core::Serilization::Serilize(tuple);
	decltype(tuple) deserelizedTuple;

	Core::Serilization::Deserilize(stream, deserelizedTuple);

	return 
		tuple.GetByIdx<0>() == deserelizedTuple.GetByIdx<0>() &&
		[&](){
		auto array = tuple.GetByIdx<1>();
		auto deserelizedArray = deserelizedTuple.GetByIdx<1>();
		if(array.Count != deserelizedArray.Count)
			return false;
		for (SizeT i = 0; i < array.Count; i++)
		{
			if(array[i] != deserelizedArray[i])
				return false;
		}
		return true;}
		() && [&](){
			auto list = tuple.GetByIdx<2>();
			auto deserelizedList = deserelizedTuple.GetByIdx<2>();
			if(list.Count != deserelizedList.Count)
				return false;	
			auto it = list.FirstIt;
			auto deserelizedIt = deserelizedList.FirstIt;
			while(it != nullptr){
				if(*it != *deserelizedIt)
					return false;
				it++; deserelizedIt++;
			}
			return true;
		}()&&
		tuple.GetByIdx<3>() == deserelizedTuple.GetByIdx<3>();
}

bool Seed::Core::Testing::Test::Serilization::TupleTypeTupleSerilization::Excecute() {
	auto tuple = Seed::Core::Containers::Tuple<
		Seed::Core::Containers::Tuple<Int32, Int64, Boolean>,
		Seed::Core::Containers::Tuple<Seed::Core::Containers::Array<Int32>>,
		Boolean>{
			{1,1,true},
	{{1,2,3,4}},
	true
	};

	auto stream = Core::Serilization::Serilize(tuple);
	decltype(tuple) deserelizedTuple;

	Core::Serilization::Deserilize(stream, deserelizedTuple);

	return 
		[&](){
		auto item1 = tuple.GetByIdx<0>();
		auto deserelizedItem1 = deserelizedTuple.GetByIdx<0>();
		return 
			item1.GetByIdx<0>() == deserelizedItem1.GetByIdx<0>() &&
			item1.GetByIdx<1>() == deserelizedItem1.GetByIdx<1>() &&
			item1.GetByIdx<2>() == deserelizedItem1.GetByIdx<2>();
	}()
		&&
	[&](){
		auto item1 = tuple.GetByIdx<1>();
		auto deserelizedItem1 = deserelizedTuple.GetByIdx<1>();
		return 
			[&](){ //-V1047
		auto array = item1.GetByIdx<0>();
		auto deserelizedArray = deserelizedItem1.GetByIdx<0>();
		if(array.Count != deserelizedArray.Count)
			return false;
		for (SizeT i = 0; i < array.Count; i++)
		{
			if(array[i] != deserelizedArray[i])
				return false;
		}
		return true;
		}();
	}()
		&&
		tuple.GetByIdx<2>() == deserelizedTuple.GetByIdx<2>();
}

bool Seed::Core::Testing::Test::Serilization::CustomStructSerilization::Excecute() {
	auto data = Core::CTTI::TypeTraits::SerilizableClass();
	data.Name() = L"Test";
	data.Storage() = {{L"one", L"1"}, {L"two", L"2"}, {L"three", L"3"}};

	auto stream = Core::Serilization::Serilize(data);
	decltype(data) deserelizedData;

	Core::Serilization::Deserilize(stream, deserelizedData);

	return data.Name() == deserelizedData.Name() &&
		[&](){
	
		if(data.Storage().Count != deserelizedData.Storage().Count)
			return false;

		bool result = true;

		data.Storage().ForEach([&](const Core::Containers::Pair<String, String> & dataItem){
			bool found = false;
			deserelizedData.Storage().ForEach([&](const Core::Containers::Pair<String, String> & deserelizedDataItem){
				if(dataItem == deserelizedDataItem)
					if(!found)
						found = true;
			});
			if(!found)
				result = false;
		});
		return true;
	}();
}
