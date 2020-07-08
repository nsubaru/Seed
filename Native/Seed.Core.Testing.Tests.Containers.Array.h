#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed 
{
	inline namespace Core 
	{
		namespace Testing 
		{
			namespace Test 
			{
				namespace API
				{ 
					namespace Containers
					{
						namespace Array
						{
							namespace NonConcurency
							{
								class ForEachTestArray : public TestClass<ForEachTestArray> {
									virtual bool Excecute() override;
								};

								class ForEachFromTestArray : public TestClass<ForEachTestArray> {
									virtual bool Excecute() override;
								};

								class ForEachFromToTestArray : public TestClass<ForEachTestArray> {
									virtual bool Excecute() override;
								};

								class AgregateSimpleItemsTypeTestArray : public TestClass<AgregateSimpleItemsTypeTestArray> {
									virtual bool Excecute() override;
								};

								class AgregateConvertionsItemsTypeTestArray : public TestClass<AgregateSimpleItemsTypeTestArray> {
									virtual bool Excecute() override;
								};

								class AnyEmptyArray : public TestClass<AnyEmptyArray> {
									virtual bool Excecute() override;
								};

								class AnyWithItemArray : public TestClass<AnyWithItemArray> {
									virtual bool Excecute() override;
								};

								class AnyWithConditionArray : public TestClass<AnyWithConditionArray> {
									virtual bool Excecute() override;
								};

								class AllWithConditionArray : public TestClass<AllWithConditionArray> {
									virtual bool Excecute() override;
								};

								class CloneArrayWithPods : public TestClass<CloneArrayWithPods> {
									virtual bool Excecute() override;
								};

								class CloneArrayWithClonableItems : public TestClass<CloneArrayWithClonableItems> {
									virtual bool Excecute() override;
								};

								class ResizeArray : public TestClass<ResizeArray> {
									virtual bool Excecute() override;
								};

								class FirstConditionArray : public TestClass<FirstConditionArray> {
									virtual bool Excecute() override;
								};

								class LastConditionArray : public TestClass<LastConditionArray> {
									virtual bool Excecute() override;
								};
							}

							namespace Concurency
							{
								class ForEachTestArray : public TestClass<ForEachTestArray> {
									virtual bool Excecute() override;
								};

								class ForEachFromTestArray : public TestClass<ForEachTestArray> {
									virtual bool Excecute() override;
								};

								class ForEachFromToTestArray : public TestClass<ForEachTestArray> {
									virtual bool Excecute() override;
								};

								class AgregateSimpleItemsTypeTestArray : public TestClass<AgregateSimpleItemsTypeTestArray> {
									virtual bool Excecute() override;
								};

								class AgregateConvertionsItemsTypeTestArray : public TestClass<AgregateSimpleItemsTypeTestArray> {
									virtual bool Excecute() override;
								};

								class AnyEmptyArray : public TestClass<AnyEmptyArray> {
									virtual bool Excecute() override;
								};

								class AnyWithItemArray : public TestClass<AnyWithItemArray> {
									virtual bool Excecute() override;
								};

								class AnyWithConditionArray : public TestClass<AnyWithConditionArray> {
									virtual bool Excecute() override;
								};

								class AllWithConditionArray : public TestClass<AllWithConditionArray> {
									virtual bool Excecute() override;
								};

								class CloneArrayWithPods : public TestClass<CloneArrayWithPods> {
									virtual bool Excecute() override;
								};

								class CloneArrayWithClonableItems : public TestClass<CloneArrayWithClonableItems> {
									virtual bool Excecute() override;
								};

								class ResizeArray : public TestClass<ResizeArray> {
									virtual bool Excecute() override;
								};

								class FirstConditionArray : public TestClass<FirstConditionArray> {
									virtual bool Excecute() override;
								};

								class LastConditionArray : public TestClass<LastConditionArray> {
									virtual bool Excecute() override;
								};
							}
						}
					}
				}
			}
		}
	}
}