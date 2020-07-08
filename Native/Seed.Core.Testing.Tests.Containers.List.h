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
						namespace List
						{
							namespace NonConcurency
							{
								class ForEachTestList : public TestClass<ForEachTestList> {
									virtual bool Excecute() override;
								};

								class RemoveIfTestList : public TestClass<RemoveIfTestList> {
									virtual bool Excecute() override;
								};

								class AgregateSimpleItemsTypeTestList : public TestClass<AgregateSimpleItemsTypeTestList> {
									virtual bool Excecute() override;
								};

								class AgregateConvertionsItemsTypeTestList : public TestClass<AgregateSimpleItemsTypeTestList> {
									virtual bool Excecute() override;
								};

								class AnyEmptyList : public TestClass<AnyEmptyList> {
									virtual bool Excecute() override;
								};

								class AnyWithItemList : public TestClass<AnyWithItemList> {
									virtual bool Excecute() override;
								};

								class AnyWithConditionList : public TestClass<AnyWithConditionList> {
									virtual bool Excecute() override;
								};

								class AllWithConditionList : public TestClass<AllWithConditionList> {
									virtual bool Excecute() override;
								};

								class CloneListWithPods : public TestClass<CloneListWithPods> {
									virtual bool Excecute() override;
								};

								class CloneListWithClonableItems : public TestClass<CloneListWithClonableItems> {
									virtual bool Excecute() override;
								};

								class FirstConditionList : public TestClass<FirstConditionList> {
									virtual bool Excecute() override;
								};

								class LastConditionList : public TestClass<LastConditionList> {
									virtual bool Excecute() override;
								};

								class ListIteratorsLoop : public TestClass<ListIteratorsLoop> {
									virtual bool Excecute() override;
								};
							}

							namespace Concurency
							{
								class ForEachTestList : public TestClass<ForEachTestList> {
									virtual bool Excecute() override;
								};

								class RemoveIfTestList : public TestClass<RemoveIfTestList> {
									virtual bool Excecute() override;
								};

								class AgregateSimpleItemsTypeTestList : public TestClass<AgregateSimpleItemsTypeTestList> {
									virtual bool Excecute() override;
								};

								class AgregateConvertionsItemsTypeTestList : public TestClass<AgregateSimpleItemsTypeTestList> {
									virtual bool Excecute() override;
								};

								class AnyEmptyList : public TestClass<AnyEmptyList> {
									virtual bool Excecute() override;
								};

								class AnyWithItemList : public TestClass<AnyWithItemList> {
									virtual bool Excecute() override;
								};

								class AnyWithConditionList : public TestClass<AnyWithConditionList> {
									virtual bool Excecute() override;
								};

								class AllWithConditionList : public TestClass<AllWithConditionList> {
									virtual bool Excecute() override;
								};

								class CloneListWithPods : public TestClass<CloneListWithPods> {
									virtual bool Excecute() override;
								};

								class CloneListWithClonableItems : public TestClass<CloneListWithClonableItems> {
									virtual bool Excecute() override;
								};

								class FirstConditionList : public TestClass<FirstConditionList> {
									virtual bool Excecute() override;
								};

								class LastConditionList : public TestClass<LastConditionList> {
									virtual bool Excecute() override;
								};

								class ListIteratorsLoop : public TestClass<ListIteratorsLoop> {
									virtual bool Excecute() override;
								};
							}

							namespace MultiThreadConcurency
							{
								class IteratorsLoop : public TestClass<IteratorsLoop> {
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