// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing,Tests.Serilization.CollectionTests.h"

bool Seed::Core::Testing::Test::Serilization::StringSerilization::Excecute() {
    String Value = __FUNCSIG__;
    const auto stream = Core::Serilization::Serilize(Value);
    decltype(Value) deserelizedValue;
     Core::Serilization::Deserilize(stream, deserelizedValue);
    return Value == deserelizedValue;
}

bool Seed::Core::Testing::Test::Serilization::ArraySerilization::Excecute() {
	auto array = Containers::Array<Containers::Array<SizeT>>::Generate([](){
			return Containers::Array<SizeT>::Generate([](SizeT Index){
				return Index * 10;
			}, 100);
		}, 100);

	const auto stream = Core::Serilization::Serilize(array);
	decltype(array) deserelizedArray;
	Core::Serilization::Deserilize(stream, deserelizedArray);
	for (SizeT i = 0; i < 100; i++)
	{
		for (SizeT j = 0; j < 100; j++)
		{
			if(array[i][j] != deserelizedArray[i][j])
				return false;
		}
	}
	return true;

}

bool Seed::Core::Testing::Test::Serilization::ListSerilization::Excecute() {
		auto list = Containers::List<Containers::List<SizeT>>::Generate([](){
			return Containers::List<SizeT>::Generate([](SizeT Index){
				return Index * 10;
			}, 100);
		}, 100);

		const auto stream =  Core::Serilization::Serilize(list);
	decltype(list) deserelizedList;
	 Core::Serilization::Deserilize(stream, deserelizedList);

	if(list.Count != deserelizedList.Count)
		return false;

	auto it = list.FirstIt;
	auto itDeserilized = deserelizedList.FirstIt;

	while(it!=nullptr){
		if(it->Count != itDeserilized->Count)
			return false;

		auto innerIt = it->FirstIt;
		auto innerItDeserilized = itDeserilized->FirstIt;
		while(innerIt!=nullptr)
		{
			if(*innerIt != *innerItDeserilized)
				return false;

			++innerIt;
			++innerItDeserilized;
		}

		++it;
		++itDeserilized;
	}
	return true;
}

bool Seed::Testing::Test::Serilization::DynamicQueue::Excecute()
{
	auto queue = Containers::DynamicQueue<Containers::DynamicQueue<SizeT>>::Generate([](){
		return Containers::DynamicQueue<SizeT>::Generate([](SizeT index){
			return index * 10;
		}, 100);
	}, 100);

	auto stream = Core::Serilization::Serilize(queue);
	decltype(queue) deserelizedQueue;
	Core::Serilization::Deserilize(stream, deserelizedQueue);

	if(queue.Count != deserelizedQueue.Count)
		return false;

	while(queue.Count != 0)
	{
		auto queueItem = queue.Dequeue();
		auto deserelizedQueueItem = deserelizedQueue.Dequeue();

		if(queueItem.Count != deserelizedQueueItem.Count)
			return false;

		while(queueItem.Count != 0)
		{
			if(queueItem.Dequeue() != deserelizedQueueItem.Dequeue())
				return false;
		}
	}

	return true;
}

bool Seed::Testing::Test::Serilization::FixedArray::Excecute()
{
	auto array = Core::Containers::FixedArray<10, Core::Containers::FixedArray<10, SizeT>>::Generate([](SizeT i)
		{
			return Core::Containers::FixedArray<10, SizeT>::Generate([&](SizeT j)
				{
					return i * 10 + j;
				});
		});

	const auto stream = Core::Serilization::Serilize(array);

	decltype(array) deserelizedArray;
	Core::Serilization::Deserilize(stream, deserelizedArray);

	if(array.Count != deserelizedArray.Count)
		return false;

	for (SizeT i = 0; i < array.Count; i++)
	{
		for (SizeT j = 0; j < array[i].Count; j++)
		{
			if(array[i][j] != deserelizedArray[i][j])
				return false;
		}
	}

	return true;
}

bool Seed::Testing::Test::Serilization::FixedQueue::Excecute()
{
	auto queue = Core::Containers::FixedQueue<10, Core::Containers::FixedQueue<10, Int32>>{
		{1,2,3,4,5},
		{1,2,3,4,5},
		{1,2,3,4,5},
		{1,2,3,4,5}
	};

	const auto stream = Core::Serilization::Serilize(queue);

	decltype(queue) deserelizedQueue;
	Core::Serilization::Deserilize(stream, deserelizedQueue);

	if(queue.Count != deserelizedQueue.Count)
		return false;

	while(queue.Count != 0)
	{
		auto innerQueue = queue.Dequeue();
		auto innerDeserelizedQueue = deserelizedQueue.Dequeue();

		if(innerQueue.Count != innerDeserelizedQueue.Count)
			return false;

		while(innerQueue.Count != 0)
		{
			if(innerQueue.Dequeue() != innerDeserelizedQueue.Dequeue())
				return false;
		}
	}

	return true;
}

bool Seed::Testing::Test::Serilization::Dictionary::Excecute()
{
	auto dictionary = Core::Containers::Dictionary<Int32>{
		{L"один", 1},
		{L"два", 2},
		{L"три", 3},
		{L"чотири", 4},
		{L"п'ять", 5},
		{L"шість", 6},
		{L"сім", 7},
		{L"вісім", 8},
		{L"дев'ять", 9},
	};

	const auto stream = Core::Serilization::Serilize(dictionary);
	decltype(dictionary) deserelizedDictionary;

	Core::Serilization::Deserilize(stream,deserelizedDictionary);

	if(dictionary.Count != deserelizedDictionary.Count)
		return false;

	auto IsAllFound = true;
	
	dictionary.ForEach([&](const Containers::Pair<String, Int32> & dictionaryPair)
		{
			auto flag = false;
			deserelizedDictionary.ForEach([&](const Containers::Pair<String, Int32> &deserelizedDictionaryPair)
				{
					if(!flag){
						if(dictionaryPair == deserelizedDictionaryPair)
						{
							flag = true;
						}
					}
				});
			IsAllFound = (IsAllFound) ? flag : false;  
		});

	return IsAllFound;
}

bool Seed::Testing::Test::Serilization::Map::Excecute()
{
	auto map = Core::Containers::Map<String, Int32>{
		{L"один", 1},
		{L"два", 2},
		{L"три", 3},
		{L"чотири", 4},
		{L"п'ять", 5},
		{L"шість", 6},
		{L"сім", 7},
		{L"вісім", 8},
		{L"дев'ять", 9},
	};

	const auto stream = Core::Serilization::Serilize(map);
	decltype(map) deserelizedMap;

	Core::Serilization::Deserilize(stream,deserelizedMap);

	if(map.Count != deserelizedMap.Count)
		return false;

	auto itMap = map.FirstIt();
	auto itDeserelizedMap = deserelizedMap.FirstIt();

	while(itMap != nullptr)
	{
		if(*itMap != *itDeserelizedMap)
			return false;

		++itMap;
		++itDeserelizedMap;
	}

	return true;
}

bool Seed::Testing::Test::Serilization::PriorityQueue::Excecute()
{
	auto priortyQueue = Containers::PriorityQueue<Int32, String>{
		{1, "один"},
		{1, "два"},
		{1, "три"},
		{1, "чотири"},
		{1, "п'ять"},
		{2, "шість"},
		{2, "сім"},
		{2, "вісім"},
		{2, "дев'ять"},
		{2, "десять"}
	};

	const auto stream = Core::Serilization::Serilize(priortyQueue);
	decltype(priortyQueue) deserelizedPriortyQueue;

	Core::Serilization::Deserilize(stream,deserelizedPriortyQueue);

	if(priortyQueue.Count != deserelizedPriortyQueue.Count)
		return false;

	auto IsAllFound = true;
	
	priortyQueue.ForEach([&](const Containers::Pair<Int32, String> & dictionaryPair)
		{
			auto flag = false;
			deserelizedPriortyQueue.ForEach([&](const Containers::Pair<Int32, String> &deserelizedDictionaryPair)
				{
					if(!flag){
						if(dictionaryPair == deserelizedDictionaryPair)
						{
							flag = true;
						}
					}
				});
			IsAllFound = (IsAllFound) ? flag : false;  
		});

	return IsAllFound;
}

