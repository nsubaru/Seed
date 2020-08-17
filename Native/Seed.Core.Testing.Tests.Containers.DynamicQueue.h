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
						namespace DynamicQueue
						{
							namespace NonConcurency
							{
								class ForEachTestDynamicQueue : public TestClass<ForEachTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class ForEachToTestDynamicQueue : public TestClass<ForEachToTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AgregateSimpleItemsTypeTestDynamicQueue : public TestClass<AgregateSimpleItemsTypeTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AgregateConvertionsItemsTypeTestDynamicQueue : public TestClass<AgregateConvertionsItemsTypeTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AnyEmptyDynamicQueue : public TestClass<AnyEmptyDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AnyWithItemDynamicQueue : public TestClass<AnyWithItemDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AnyWithConditionDynamicQueue : public TestClass<AnyWithConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AllWithConditionDynamicQueue : public TestClass<AllWithConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class CloneDynamicQueueWithPods : public TestClass<CloneDynamicQueueWithPods> {
									virtual bool Excecute() override;
								};

								class CloneDynamicQueueWithClonableItems : public TestClass<CloneDynamicQueueWithClonableItems> {
									virtual bool Excecute() override;
								};

								class ShrinkDynamicQueue : public TestClass<ShrinkDynamicQueue> {
									virtual bool Excecute() override;
								};

								class ExtendDynamicQueue : public TestClass<ExtendDynamicQueue> {
									virtual bool Excecute() override;
								};

								class FirstConditionDynamicQueue : public TestClass<FirstConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class LastConditionDynamicQueue : public TestClass<LastConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class EqueuDynamicQueue : public TestClass<EqueuDynamicQueue> {
									virtual bool Excecute() override;
								};

								class DequeuDynamicQueue : public TestClass<DequeuDynamicQueue> {
									virtual bool Excecute() override;
								};

								class TopDynamicQueue : public TestClass<TopDynamicQueue> {
									virtual bool Excecute() override;
								};

								class MaxItem : public TestClass<MaxItem> {
									virtual bool Excecute() override;
								};

								class MaxItemField : public TestClass<MaxItemField> {
									virtual bool Excecute() override;
								};

								class MaxByItemField : public TestClass<MaxByItemField> {
									virtual bool Excecute() override;
								};

								class MinItem : public TestClass<MinItem> {
									virtual bool Excecute() override;
								};

								class MinItemField : public TestClass<MinItemField> {
									virtual bool Excecute() override;
								};

								class MinByItemField : public TestClass<MinByItemField> {
									virtual bool Excecute() override;
								};
							}

							namespace Concurency
							{
								class ForEachTestDynamicQueue : public TestClass<ForEachTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class ForEachToTestDynamicQueue : public TestClass<ForEachToTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AgregateSimpleItemsTypeTestDynamicQueue : public TestClass<AgregateSimpleItemsTypeTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AgregateConvertionsItemsTypeTestDynamicQueue : public TestClass<AgregateConvertionsItemsTypeTestDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AnyEmptyDynamicQueue : public TestClass<AnyEmptyDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AnyWithItemDynamicQueue : public TestClass<AnyWithItemDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AnyWithConditionDynamicQueue : public TestClass<AnyWithConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class AllWithConditionDynamicQueue : public TestClass<AllWithConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class CloneDynamicQueueWithPods : public TestClass<CloneDynamicQueueWithPods> {
									virtual bool Excecute() override;
								};

								class CloneDynamicQueueWithClonableItems : public TestClass<CloneDynamicQueueWithClonableItems> {
									virtual bool Excecute() override;
								};

								class ShrinkDynamicQueue : public TestClass<ShrinkDynamicQueue> {
									virtual bool Excecute() override;
								};

								class ExtendDynamicQueue : public TestClass<ExtendDynamicQueue> {
									virtual bool Excecute() override;
								};

								class FirstConditionDynamicQueue : public TestClass<FirstConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class LastConditionDynamicQueue : public TestClass<LastConditionDynamicQueue> {
									virtual bool Excecute() override;
								};

								class EqueuDynamicQueue : public TestClass<EqueuDynamicQueue> {
									virtual bool Excecute() override;
								};

								class DequeuDynamicQueue : public TestClass<DequeuDynamicQueue> {
									virtual bool Excecute() override;
								};

								class TopDynamicQueue : public TestClass<TopDynamicQueue> {
									virtual bool Excecute() override;
								};

								class MultiThreadQueueProcessingDynamicQueue : public TestClass<MultiThreadQueueProcessingDynamicQueue> {
									virtual bool Excecute() override;
								};

								class MaxItem : public TestClass<MaxItem> {
									virtual bool Excecute() override;
								};

								class MaxItemField : public TestClass<MaxItemField> {
									virtual bool Excecute() override;
								};

								class MaxByItemField : public TestClass<MaxByItemField> {
									virtual bool Excecute() override;
								};

								class MinItem : public TestClass<MinItem> {
									virtual bool Excecute() override;
								};

								class MinItemField : public TestClass<MinItemField> {
									virtual bool Excecute() override;
								};

								class MinByItemField : public TestClass<MinByItemField> {
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